# ISS-Notify Firmware

[![travis-ci](https://travis-ci.org/open-notify/ISS-Notify-Firmware.svg)](https://travis-ci.org/open-notify/ISS-Notify-Firmware)

Firmware for ISS Notify


### Requriements:

Make sure you have `gcc` installed along with the gcc AVR cross-compiler toolchain:

    $ sudo apt-get install gcc gcc-avr avr-libc

Be sure the initilize the LUFA submodule

    $ git submodule update --init


### Build:

Make both the ISS-Notify board project, and the Test Harness project:

    $ make build


### Clean:

    $ make clean


## Documentation

[open-notify.org](http://open-notify.org)


# License

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
