// src/main.cpp
#include <iostream>
#include "libusb.h" // Include the libusb header

int main() {
    libusb_context* ctx = nullptr;
    int result = libusb_init(&ctx); // Initialize libusb

    if (result < 0) {
        std::cerr << "Error initializing libusb: " << libusb_error_name(result) << std::endl;
        return 1;
    }

    std::cout << "libusb initialized successfully!" << std::endl;
    std::cout << "Running application code..." << std::endl;
    // --- Add your application logic that uses libusb here ---

    libusb_exit(ctx); // Deinitialize libusb
    std::cout << "libusb context closed." << std::endl;

    return 0;
}