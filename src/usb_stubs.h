#ifndef USB_STUBS_H
#define USB_STUBS_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// Minimal libusb type definitions
typedef struct libusb_context libusb_context;
typedef struct libusb_device_handle libusb_device_handle;

// Minimal libusb error codes
enum libusb_error {
    LIBUSB_SUCCESS = 0,
    LIBUSB_ERROR_IO = -1,
    LIBUSB_ERROR_INVALID_PARAM = -2,
    LIBUSB_ERROR_ACCESS = -3,
    LIBUSB_ERROR_NO_DEVICE = -4,
    LIBUSB_ERROR_NOT_FOUND = -5,
    LIBUSB_ERROR_BUSY = -6,
    LIBUSB_ERROR_TIMEOUT = -7,
    LIBUSB_ERROR_OVERFLOW = -8,
    LIBUSB_ERROR_PIPE = -9,
    LIBUSB_ERROR_INTERRUPTED = -10,
    LIBUSB_ERROR_NO_MEM = -11,
    LIBUSB_ERROR_NOT_SUPPORTED = -12,
    LIBUSB_ERROR_OTHER = -99,
};

// Minimal libusb descriptor types
#define LIBUSB_DT_DEVICE 0x01
#define LIBUSB_DT_CONFIG 0x02
#define LIBUSB_DT_STRING 0x03
#define LIBUSB_DT_INTERFACE 0x04
#define LIBUSB_DT_ENDPOINT 0x05
#define LIBUSB_DT_BOS 0x0f
#define LIBUSB_DT_DEVICE_CAPABILITY 0x10
#define LIBUSB_DT_HID 0x21
#define LIBUSB_DT_REPORT 0x22
#define LIBUSB_DT_PHYSICAL 0x23
#define LIBUSB_DT_HUB 0x29
#define LIBUSB_DT_SUPERSPEED_HUB 0x2a
#define LIBUSB_DT_SS_ENDPOINT_COMPANION 0x30


// Function declarations
int libusb_init(libusb_context **ctx);
void libusb_exit(libusb_context *ctx);
libusb_device_handle *libusb_open_device_with_vid_pid(libusb_context *ctx, uint16_t vendor_id, uint16_t product_id);
void libusb_close(libusb_device_handle *dev_handle);
int libusb_set_auto_detach_kernel_driver(libusb_device_handle *dev_handle, int enable);
int libusb_claim_interface(libusb_device_handle *dev_handle, int interface_number);
int libusb_get_descriptor(libusb_device_handle *dev_handle, uint8_t desc_type, uint8_t desc_index, unsigned char *data, int length);
int libusb_interrupt_transfer(libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *actual_length, unsigned int timeout);
const char *libusb_error_name(int errcode);

#ifdef __cplusplus
}
#endif

#endif // USB_STUBS_H
