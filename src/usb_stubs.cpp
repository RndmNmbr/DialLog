#include "usb_stubs.h"
#include <iostream> // For printf-style debugging, can be removed later
#include <cstring> // For strcpy

// Define dummy structures for opaque pointers
struct libusb_context {};
struct libusb_device_handle {};

static struct libusb_context dummy_context;
static struct libusb_device_handle dummy_device_handle;

int libusb_init(libusb_context **ctx) {
    if (ctx) {
        *ctx = &dummy_context; // Assign a dummy non-null value
    }
    std::cout << "libusb_init called" << std::endl;
    return LIBUSB_SUCCESS;
}

void libusb_exit(libusb_context *ctx) {
    // No-op
    std::cout << "libusb_exit called" << std::endl;
}

libusb_device_handle *libusb_open_device_with_vid_pid(libusb_context *ctx, uint16_t vendor_id, uint16_t product_id) {
    std::cout << "libusb_open_device_with_vid_pid called with VID: " << vendor_id << " PID: " << product_id << std::endl;
    return &dummy_device_handle; // Return a dummy non-null handle
}

void libusb_close(libusb_device_handle *dev_handle) {
    // No-op
    std::cout << "libusb_close called" << std::endl;
}

int libusb_set_auto_detach_kernel_driver(libusb_device_handle *dev_handle, int enable) {
    std::cout << "libusb_set_auto_detach_kernel_driver called with enable: " << enable << std::endl;
    return LIBUSB_SUCCESS;
}

int libusb_claim_interface(libusb_device_handle *dev_handle, int interface_number) {
    std::cout << "libusb_claim_interface called for interface: " << interface_number << std::endl;
    return LIBUSB_SUCCESS;
}

int libusb_get_descriptor(libusb_device_handle *dev_handle, uint8_t desc_type, uint8_t desc_index, unsigned char *data, int length) {
    std::cout << "libusb_get_descriptor called for type: " << (int)desc_type << " index: " << (int)desc_index << " length: " << length << std::endl;
    // For now, just return 0 bytes written.
    // To simulate getting a device descriptor, one could copy some dummy data:
    if (desc_type == LIBUSB_DT_DEVICE && length >= 18) { // 18 bytes is the size of a standard device descriptor
        // Example: Fill with minimal valid-looking data if needed for basic parsing
        // data[0] = 18; // bLength
        // data[1] = LIBUSB_DT_DEVICE; // bDescriptorType
        // ...
        // return 18; // Actual length
    }
    return 0; 
}

int libusb_interrupt_transfer(libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *actual_length, unsigned int timeout) {
    std::cout << "libusb_interrupt_transfer called for endpoint: " << (int)endpoint << " length: " << length << " timeout: " << timeout << std::endl;
    if (actual_length) {
        *actual_length = 0; // Simulate no data transferred
    }
    // Alternate between success and timeout to test loop conditions
    static int call_count = 0;
    if (call_count++ % 2 == 0) {
         //std::cout << "libusb_interrupt_transfer returning LIBUSB_SUCCESS" << std::endl;
        return LIBUSB_SUCCESS; 
    } else {
        //std::cout << "libusb_interrupt_transfer returning LIBUSB_ERROR_TIMEOUT" << std::endl;
        return LIBUSB_ERROR_TIMEOUT;
    }
}

const char *libusb_error_name(int errcode) {
    std::cout << "libusb_error_name called for errcode: " << errcode << std::endl;
    // It's better to return a static string literal
    static const char* stub_error_str = "STUB_ERROR";
    static const char* success_str = "STUB_SUCCESS";
    static const char* timeout_str = "STUB_TIMEOUT";

    switch(errcode) {
        case LIBUSB_SUCCESS:
            return success_str;
        case LIBUSB_ERROR_TIMEOUT:
            return timeout_str;
        default:
            return stub_error_str;
    }
}
