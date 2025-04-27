# DialLog - "Logging precision, not keypresses!"

[![Build Windows Application](https://github.com/RndmNmbr/DialLog/actions/workflows/build.yaml/badge.svg)](https://github.com/RndmNmbr/DialLog/actions/workflows/build.yaml)

Direct logging for precision measuring instruments that identify as USB HID devices

## Overview
DialLog is a lightweight Windows utility for capturing and logging USB HID output from digital dial indicators and similar precision measurement tools. When the user replaces the default HID driver with libusbK via Zadig (or by hand if you are a masochist), DialLog allows the scan code data to be read directly.  This means that the instrument's output does not act as keyboard data to Windows.  The output is logged to the console and a file for easy analysis.

This was developed around the DIGR-0105 USB-capable dial indicator from Clockwise Tools, with the DTCR-01 cable attached.  This is described as an "RS232" cable - but it identifies as a HID device.

https://clockwisetools.com/collections/clockwise-tools/products/clockwise-tools-digr-0105-digital-indicator-0-1-25-4mm?variant=18461726376032

https://clockwisetools.com/products/clockwise-tools-dtcr-01-rs232-data-cable?_pos=20&_sid=dc08b5326&_ss=r&variant=18590861492320

Although the tool was developed and tested specifically for this brand of devices, it has a high probability of working seamlessly with other brands of instruments.  HID output for precision measurement devices such as calipers, dial indicators, DTIs, and micrometers seems very common.

This application expects the output data to consist of a floating-point number terminated by the "ENTER" key.  Adapting it to other formats wouldn't be very hard if required - pull requests are welcome. If someone wants to donate me some Mitutoyo, Starrett, Fowler, or even SPI stuff to integrate and test with, feel free! :-) (I have only needed to automate the dial indicator so far ... so that's all I own that's got a data port.)

## Usage Instructions

This utility isn't designed to be super easy to use.  It's not hard or anything, but I took the UNIX philosphy here and used other tools in the "pipeline" that are already good at their jobs. Therefore it doesn't (currently) have nifty functions like USB device scanning, drop down menus, etc.

It is, however, statically linked - so, yeah, I at least did that for you.

The default behavior is to output a CSV with measurement records for each complete measurement received.  Each CSV line contains the following elements:
1. Time stamp.  It's in UTC UNIX epoch time.
1. Measurement value. This is stored internally as a double and just printed.
1. Raw keystroke->character buffer. Just in case something went wrong with the conversion to double, you can at least see what the application received from the instrument.

### Usage for the Clockwise DTCR-01

Using the tool with the same Clockwise instrument I use should be pretty easy.  If your instrument has a USB Vendor ID (VID) of 0x413D and a USB Product ID (PID) of 0x553A, then there are only two steps:
1. Use [Zadig](https://zadig.akeo.ie) to change the driver your device to libusbK.  Make sure you get the right device. Seriously. Make sure. While you are here, you can tag that "Edit" box and change the name to reflect what it actually is.
![Zadig screenshot](/docs/images/Zadig.png)
1. Run the DialLog tool
1. Profit

### Usage for anything else

Using the tool with other devices requires a bit more knowledge. The default VID, PID, HID interface number, and endpoint interrupt aren't going to work. Probably. Maybe.  THe VID/PID almost certainly not.

So what is one to do? You can get the VID and PID from Zadig or Windows easily enough. The other stuff though, is easiest with specialized tooling. I like [USB Device Tree Viewer](https://www.uwe-sieber.de/usbtreeview_e.html) for this. It'll show you everyting you never wanted to know about USB on your Windows system.
![USB Detailed Info](/docs/images/UsbDeviceTreeViewer.png)

Once you have the information from this tool for the device you are trying to interface with, you should have everything you need to provide command line arguments to DialLog overriding its defaults and forcing it to do your bidding.