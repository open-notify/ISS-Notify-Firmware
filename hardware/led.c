/**
 * @file
 * @author  Nathan Bergey <nathan.bergey@gmail.com>
 * @version Thursday, May 03 2012
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief LED Driver
 *
 * @section DESCRIPTION
 *
 * Code that manages talking to the LED drivers
 */

#include <avr/io.h>
#include "led.h"

void write_word(uint16_t word, uint8_t len);

void LED_Init(void)
{
	LED_INIT;
	SIN_LOW;
	SCLK_LOW;
}

void write_word(uint16_t word, uint8_t len)
{
	int8_t i;

	for (i=len-1;i>=0;i--)
	{
		SIN_LOW;

		// Bit
		if ((word & (1 << i)) != 0)
			SIN_HIGH;

		// Clock
		SCLK_HIGH;
		SCLK_LOW;
	}
	SIN_LOW;
}

void led(uint16_t *lights)
{
	// Write command
	write_word(0x25, 6);

	// Config
	write_word(0x16, 5);

	// BC
	write_word(0x7f, 7);  //Blue
	write_word(0x7f, 7);  //Green
	write_word(0x7f, 7);  //Red

	// Brightness
	write_word(0x8000, 16);  //Blue3
	write_word(0x8000, 16);  //Green3
	write_word(0x8000, 16);  //Red3

	write_word(0x8000, 16);  //Blue2
	write_word(0x0000, 16);  //Green2
	write_word(0x0000, 16);  //Red2

	write_word(0x8000, 16);  //Blue1
	write_word(0x0100, 16);  //Green1
	write_word(0x0010, 16);  //Red1

	write_word(0x0000, 16);  //Blue0
	write_word(0x8000, 16);  //Green0
	write_word(0x0000, 16);  //Red0
}
