# ISS-Notify Firmware

This has all the code that gets flashed to the ISS-Notify board.

### Requriements:

    $ sudo apt-get install gcc-avr avr-libc

And for the host loader app

    $ sudo apt-get install libusb-dev
    $ cd HostLoaderApp
    $ make

### Build:

    $ make

### Clean:

    $ make clean

## Dirctory Structure

 - `hardware` (ISS-Notify board drivers)
   - LED code
   - USB code
   - RTC code
   - charge (LiPo charger) code
   - analog reading helper
 - `library` (external libraries)
   - LUFA USB library
   - I2C driver
 - `Test` (simple sample code)
   - blink
   - clock
 - ISS-Notify.c (main code)
 - Makefile (gnu make)
 - Descriptors (USB definitions)

-------------------

See open-notify.org
