# DialLog - "Logging precision, not keypresses!"
Direct logging for precision measuring instruments that identify as USB HID devices
[![Build Windows Application](https://github.com/RndmNmbr/DialLog/actions/workflows/build.yaml/badge.svg)](https://github.com/RndmNmbr/DialLog/actions/workflows/build.yaml)

DialLog is a lightweight Windows utility for capturing and logging USB HID output from digital dial indicators and similar precision measurement tools. When the user replaces the default HID driver with WinUSB via Zadig, DialLog allows the scan code data to be read directly.  This means that the instrument's output does not act as keyboard data to Windows.  The output is logged to the console and a file for easy analysis.

This was developed around the DIGR-0105 USB-capable dial indicator from Clockwise Tools, with the DTCR-01 cable attached.  This is described as an "RS232" cable - but it identifies as a HID device.

https://clockwisetools.com/collections/clockwise-tools/products/clockwise-tools-digr-0105-digital-indicator-0-1-25-4mm?variant=18461726376032
https://clockwisetools.com/products/clockwise-tools-dtcr-01-rs232-data-cable?_pos=20&_sid=dc08b5326&_ss=r&variant=18590861492320

Although the tool was developed and tested specifically for this brand of devices, it has a high probability of working seamlessly with other brands of instruments.  HID output for precision measurement devices such as calipers, dial indicators, DTIs, and micrometers seems very common.

This application expects the output data to consist of a floating-point number terminated by the "ENTER" key.  Adapting it to other formats wouldn't be very hard if required - pull requests are welcome. If someone wants to donate me some Mitutoyo, Starrett, Fowler, or even SPI stuff to integrate and test with, feel free! :-) (I have only needed to automate the dial indicator so far ... so that's all I own.)
