// Main app for DialLog instrument measurement data receiver
//
// This is pretty quick and dirty.
#include <iostream>
#include <iomanip>

#include "libusb.h" 
#include "cmdparser.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_REVISION 0



int main(int argc, char** argv) {   

    std::cerr << "DialLog - \"Logging precision, not keypresses!\"" << std::endl;
    std::cerr << "  - Version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION << ", Built " << __TIME__ << " " << __DATE__ << std::endl;

    cli::Parser parser(argc, argv);
	parser.set_optional<unsigned int>("v", "vid", 0x413D, "USB Vendor ID (VID) of the target USB device. Lead with 0x for hex input.");
	parser.set_optional<unsigned int>("p", "pid", 0x553A, "USB Product ID (PID) of the target USB device. Lead with 0x for hex input.");
	
    parser.set_optional<unsigned int>("n", "hid-if-num", 0, "HID interface number within device.");
	parser.set_optional<unsigned int>("i", "int-in-endpoint", 0x81, "Interrupt within the endpoint. Lead with 0x for hex input.");

    parser.set_optional<std::string>("o", "output", "DialLog_Output.csv", "File name to write.  Use \"stdout\" to dump to console.");
	parser.set_optional<bool>       ("t", "time-stamp", true, "Include a time stamp with each value in the output.");
	parser.run_and_exit_if_error();

    ///////////////////

    const unsigned int requestedVid = parser.get<unsigned int>("v");
    const unsigned int requestedPid = parser.get<unsigned int>("p");
    if (requestedVid > 0xFFFF)
    {
        std::cerr << "  ! Error - Requested VID is out of range." << std::endl;
        return 1;
    }

    if (requestedPid > 0xFFFF)
    {
        std::cerr << "  ! Error - Rquested PID is out of range." << std::endl;
        return 1;
    }
    std::cerr << "  - Looking for input on VID 0x" <<  std::hex << std::setw(4) << std::setfill('0') << requestedVid 
              << " PID 0x" <<  std::hex << std::setw(4) << std::setfill('0') << requestedPid << std::endl;

    const unsigned int requestedHidInterface = parser.get<unsigned int>("n");
    const unsigned int requestedIntInEndpoint = parser.get<unsigned int>("i");
    if (requestedHidInterface > 0xFF)
    {
        std::cerr << "  ! Error - Requested HID Interface is out of range." << std::endl;
        return 1;
    }

    if (requestedIntInEndpoint > 0xFF)
    {
        std::cerr << "  ! Error - Requested Interrupt within interface is out of range." << std::endl;
        return 1;
    }
    std::cerr << "  - Looking for HID interface number " << requestedHidInterface 
              << " Interrupt 0x" <<  std::hex << std::setw(2) << std::setfill('0') << requestedIntInEndpoint << std::endl;

    ///////////////////

    std::cerr << "  - libusb init ... ";
    libusb_context* pUsbContext = nullptr;
    const int initResult = libusb_init(&pUsbContext); 

    if (initResult < 0) {
        std::cerr << "Error. \"" << libusb_error_name(initResult) << "\"" <<  std::endl;
        return 1;
    }
    std::cerr << "Ok" << std::endl;

    ///////////////////
    std::cerr << "  - Opening device ... ";
    auto pUsbDevice = libusb_open_device_with_vid_pid(pUsbContext, requestedVid, requestedPid);
    if (0 == pUsbDevice) 
    { 
        std::cerr << "Error. Open failed." <<  std::endl;
        libusb_exit(pUsbContext);
        return 1;        
    }
    std::cerr << "Ok" << std::endl;

    // Detach kernel driver (optional on Windows with generic driver)
    libusb_set_auto_detach_kernel_driver(pUsbDevice, 1);

    std::cerr << "  - Claiming HID interface ... ";
    const auto claimStatus = libusb_claim_interface(pUsbDevice, requestedHidInterface);
    if (claimStatus < 0) 
    { 
        std::cerr << "Error. \"" << libusb_error_name(claimStatus) << "\"" <<  std::endl;
        libusb_close(pUsbDevice); 
        libusb_exit(pUsbContext); 
        return 1; 
    }
    std::cerr << "Ok" << std::endl;



    
    libusb_exit(pUsbContext); // Deinitialize libusb   
    return 0;
} // End main()

