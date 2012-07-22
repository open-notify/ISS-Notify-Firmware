#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "library/usb_serial.h"
#include "usb.h"

// Send a string to the USB serial port.  The string must be in
// flash memory, using PSTR
//
void send_str(const char *s)
{
	char c;
	while (1) {
		c = pgm_read_byte(s++);
		if (!c) break;
		usb_serial_putchar(c);
	}
}
