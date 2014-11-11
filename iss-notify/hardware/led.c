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
#include "structs.h"
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

void led(uint16_t (*lights)[3])
{
    uint8_t i;
	uint16_t *light;

	// Write command
	write_word(TLC_WRITE, 6);

	// Config
	write_word(TLC_TMGRST|TLC_DSPRPT, 5);

	// BC
	write_word(TLC_Blue_WB, 7);  //Blue
	write_word(TLC_Green_WB, 7);  //Green
	write_word(TLC_Red_WB, 7);  //Red

	// Colors
    for (i=0;i<4;i++)
    {
		light = lights[i];
		write_word(light[0], 16);  //Blue
		write_word(light[1], 16);  //Green
		write_word(light[2], 16);  //Red
    }

	/* Second chip */

	// Write command
	write_word(TLC_WRITE, 6);

	// Config
	write_word(TLC_TMGRST|TLC_DSPRPT, 5);

	// BC
	write_word(TLC_Blue_WB, 7);  //Blue
	write_word(TLC_Green_WB, 7);  //Green
	write_word(TLC_Red_WB, 7);  //Red

	// Colors
    for (i=4;i<8;i++)
    {
		light = lights[i];
		write_word(light[0], 16);  //Blue
		write_word(light[1], 16);  //Green
		write_word(light[2], 16);  //Red
    }
}
