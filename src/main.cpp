// Main app for DialLog instrument measurement data receiver
//
// This is pretty quick and dirty.
#include <iomanip>
#include <iostream>
#include <fstream> // Added for file operations
#include <chrono>  // Added for timestamps
#include <ctime>   // Added for timestamps
#include <cstring> // Added for memset
#include <unistd.h> // Added for usleep

#include "cmdparser.hpp"
#include "libusb.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_REVISION 0

int main(int argc, char** argv) {

  std::cerr << std::endl;
  std::cerr << "DialLog - \"Logging precision, not keypresses!\"" << std::endl;
  std::cerr << "  - Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION
            << ", Built " << __TIME__ << " " << __DATE__ << std::endl;
  std::cerr << std::endl;

  cli::Parser parser(argc, argv);
  parser.set_optional<unsigned int>(
      "v",
      "vid",
      0x413D,
      "USB Vendor ID (VID) of the target USB device. Lead with 0x for hex input.");
  parser.set_optional<unsigned int>(
      "p",
      "pid",
      0x553A,
      "USB Product ID (PID) of the target USB device. Lead with 0x for hex input.");

  parser.set_optional<unsigned int>("n", "hid-if-num", 0, "HID interface number within device.");
  parser.set_optional<unsigned int>(
      "i", "int-in-endpoint", 0x81, "Interrupt within the endpoint. Lead with 0x for hex input.");

  parser.set_optional<std::string>("o",
                                   "output",
                                   "DialLog_Output.csv",
                                   "File name to write.  Use \"stdout\" to dump to console.");
  parser.set_optional<bool>(
      "t", "time-stamp", false, "Include a time stamp with each value in the output.");
  parser.run_and_exit_if_error();

  ///////////////////

  const unsigned int requestedVid = parser.get<unsigned int>("v");
  const unsigned int requestedPid = parser.get<unsigned int>("p");
  if (requestedVid > 0xFFFF) {
    std::cerr << "  ! Error - Requested VID is out of range." << std::endl;
    return 1;
  }

  if (requestedPid > 0xFFFF) {
    std::cerr << "  ! Error - Rquested PID is out of range." << std::endl;
    return 1;
  }
  std::cerr << "  - Looking for input on VID 0x" << std::hex << std::setw(4) << std::setfill('0')
            << requestedVid << " PID 0x" << std::hex << std::setw(4) << std::setfill('0')
            << requestedPid << std::endl;

  const unsigned int requestedHidInterface = parser.get<unsigned int>("n");
  const unsigned int requestedIntInEndpoint = parser.get<unsigned int>("i");
  const unsigned int expectedReportSize = 8;
  const unsigned int readTimeoutMs = 1000;
  const unsigned int maxMeasurementBufferLength = 64 + 1;

  if (requestedHidInterface > 0xFF) {
    std::cerr << "  ! Error - Requested HID Interface is out of range." << std::endl;
    return 1;
  }

  if (requestedIntInEndpoint > 0xFF) {
    std::cerr << "  ! Error - Requested Interrupt within interface is out of range." << std::endl;
    return 1;
  }
  std::cerr << "  - Looking for HID interface number " << requestedHidInterface << " Interrupt 0x"
            << std::hex << std::setw(2) << std::setfill('0') << requestedIntInEndpoint << std::endl;
  std::cerr << "  - Expected report size is " << expectedReportSize << " bytes ..." << std::endl;
  std::cerr << "  - Read timeout is " << readTimeoutMs << " ms" << std::endl;
  std::cerr << "  - Longest measurement supported is ~" << maxMeasurementBufferLength - 1
            << " keystrokes." << std::endl;
  std::cerr << std::endl;
  ///////////////////

  std::cerr << "  - libusb init ... ";
  libusb_context* pUsbContext = nullptr;
  const int initResult = libusb_init(&pUsbContext);

  if (initResult < 0) {
    std::cerr << "Error. \"" << libusb_error_name(initResult) << "\"" << std::endl;
    return 1;
  }
  std::cerr << "Ok" << std::endl;

  ///////////////////
  std::cerr << "  - Opening device ... ";
  auto pUsbDevice = libusb_open_device_with_vid_pid(
      pUsbContext, static_cast<uint16_t>(requestedVid), static_cast<uint16_t>(requestedPid));
  if (0 == pUsbDevice) {
    std::cerr << "Error. Open failed." << std::endl;
    libusb_exit(pUsbContext);
    return 1;
  }
  std::cerr << "Ok" << std::endl;

  // Detach kernel driver (optional on Windows with generic driver)
  libusb_set_auto_detach_kernel_driver(pUsbDevice, 1);

  std::cerr << "  - Claiming HID interface ... ";
  const auto claimStatus = libusb_claim_interface(pUsbDevice, requestedHidInterface);
  if (claimStatus < 0) {
    std::cerr << "Error. \"" << libusb_error_name(claimStatus) << "\"" << std::endl;
    libusb_close(pUsbDevice);
    libusb_exit(pUsbContext);
    return 1;
  }
  std::cerr << "Ok" << std::endl;

  std::cerr << "  - Getting device descriptors ... ";
  unsigned char dummy_desc_buffer[256];

  // Both of these descriptor fetches are here primarily to help "unstick" certain devices that can
  // get hung up in the stack.
  //
  // Get the standard Device Descriptor (index 0, type 1)
  const auto descriptorFetchResult = libusb_get_descriptor(
      pUsbDevice, LIBUSB_DT_DEVICE, 0, dummy_desc_buffer, sizeof(dummy_desc_buffer));
  if (descriptorFetchResult < 0) {
    // This might fail if control transfers aren't working yet, but trying might help
    fprintf(stderr,
            "Warning: Failed to get device descriptor (error %d: %s) - Continuing anyway...\n",
            descriptorFetchResult,
            libusb_error_name(descriptorFetchResult));
  } else {
    printf("Successfully retrieved device descriptor (length %d).\n", descriptorFetchResult);
  }
  
  // unsigned char string_buffer[1000] = {0};
  // auto stringDesriptorFetchResult = libusb_get_string_descriptor_ascii(
  //     pUsbDevice,
  //     0,
  //     string_buffer,
  //     sizeof(string_buffer)); // Now proceed to the interrupt read loop...
  // if (stringDesriptorFetchResult > 0) {
  //   std::cerr << "String descriptor:" << string_buffer << std::endl;
  // }

  // For now, do very basic level poll based processing.  It's easy and pretty reliable.
  std::cerr << "  - Starting interrupt polling loop ... " << std::endl;


  char measurementBuffer[maxMeasurementBufferLength] = {0};
  unsigned int measurementBufferIndex = 0;
  while (1) {
    int actualLength = 0;
    unsigned char rawData[expectedReportSize] = {0};
    const auto transferResult = libusb_interrupt_transfer(
        pUsbDevice, requestedIntInEndpoint, rawData, sizeof(rawData), &actualLength, readTimeoutMs);

    if (LIBUSB_SUCCESS == transferResult) {
      if (expectedReportSize == actualLength) {
        const unsigned char scanCode = rawData[2]; // Keycode is in the 3rd byte

        // Ignore release reports (they are all zeros)
        if (0x00 != scanCode) {
          char receivedChar = '?';

          // Map scan codes to ASCII characters.
          // TODO: Ideally this would be a lookup table instead ...
          switch (scanCode) {
          case 0x1E:
            receivedChar = '1';
            break;
          case 0x1F:
            receivedChar = '2';
            break;
          case 0x20:
            receivedChar = '3';
            break;
          case 0x21:
            receivedChar = '4';
            break;
          case 0x22:
            receivedChar = '5';
            break;
          case 0x23:
            receivedChar = '6';
            break;
          case 0x24:
            receivedChar = '7';
            break;
          case 0x25:
            receivedChar = '8';
            break;
          case 0x26:
            receivedChar = '9';
            break;
          case 0x27:
            receivedChar = '0';
            break;
          case 0x37:
            receivedChar = '.';
            break; // Period
          case 0x2D:
            receivedChar = '-';
            break; // Minus

          case 0x28:
            receivedChar = '\n';
            break; // Enter/Newline

          default:
            std::cerr << "  ! Warning: Unhandled Scan Code: 0x" << std::hex << std::setw(2)
                      << std::setfill('0') << scanCode << std::endl;
            break;
          }

          // Process the translated character
          if ('?' != receivedChar) {
            if ('\n' == receivedChar) {
              // --- Enter detected: Process the completed measurement ---
              measurementBuffer[measurementBufferIndex] = '\0'; // Null-terminate the string
              if (measurementBufferIndex > 0) {                 // Ensure we have received something

                // Convert string to float
                const double measurementValue = atof(measurementBuffer);

                // Get output filename and timestamp flag
                const std::string outputFilename = parser.get<std::string>("o");
                const bool includeTimestamp = parser.get<bool>("t");

                if (outputFilename != "stdout") {
                  // Output to file
                  std::ofstream outFile(outputFilename, std::ios_base::app);
                  if (outFile.is_open()) {
                    if (includeTimestamp) {
                      auto now = std::chrono::system_clock::now();
                      auto now_c = std::chrono::system_clock::to_time_t(now);
                      outFile << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << ",";
                    }
                    outFile << measurementValue << std::endl;
                    outFile.close();
                  } else {
                    std::cerr << "  ! Error: Could not open output file: " << outputFilename << std::endl;
                  }
                } else {
                  // Output to stdout
                  if (includeTimestamp) {
                    auto now = std::chrono::system_clock::now();
                    auto now_c = std::chrono::system_clock::to_time_t(now);
                    // Using printf for stdout timestamp for consistency with existing printf
                    char time_buf[80];
                    std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S, ", std::localtime(&now_c));
                    printf("%s", time_buf);
                  }
                  printf("%f\n", measurementValue);
                }

              } else {
                std::cerr << "  ! Warning: ENTER key scan code received with an empty buffer."
                          << std::endl;
              }
              // Reset buffer for next reading
              measurementBufferIndex = 0;
              memset(measurementBuffer, 0, sizeof(measurementBuffer));

            } else {
              // --- Append character to buffer ---
              if (measurementBufferIndex < (maxMeasurementBufferLength - 1)) {
                measurementBuffer[measurementBufferIndex++] = receivedChar;
              } else {
                fprintf(stderr, "Error: Measurement buffer overflow!\n");
                // Reset buffer on overflow
                measurementBufferIndex = 0;
                memset(measurementBuffer, 0, sizeof(measurementBuffer));
              }
            }
          }
        } // End if (scan_code != 0x00)
      } // End if (actual_length == REPORT_SIZE)
    } else if (transferResult == LIBUSB_ERROR_TIMEOUT) {
      // No data - idle polling is fine
      usleep(10000); // Small sleep to yield CPU (10ms)
    } else {
      // Other error
      fprintf(stderr, "\nlibusb_interrupt_transfer error: %s\n", libusb_error_name(transferResult));
      break; // Exit loop on error
    }
  } // end interrupt polling while loop

  libusb_exit(pUsbContext); // Deinitialize libusb
  return 0;
} // End main()
