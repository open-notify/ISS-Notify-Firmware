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
 * @brief LED Driver Headers
 *
 * @section DESCRIPTION
 *
 * This is the header file for the LED drivers on the ISS-Notify v0.1 board. It
 * sets up the pins.
 */

/* Blank line (F5)*/
#define BLANK_HIGH  (PORTF |=  (1<<5))
#define BLANK_LOW   (PORTF &= ~(1<<5))

/* Latch line (F4)*/
#define LATCH_HIGH  (PORTF |=  (1<<4))
#define LATCH_LOW   (PORTF &= ~(1<<4))

/* Data in line (F1)*/
#define SIN_HIGH    (PORTF |=  (1<<1))
#define SIN_LOW     (PORTF &= ~(1<<1))

/* Clock line (F0)*/
#define SCLK_HIGH   (PORTF |=  (1))
#define SCLK_LOW    (PORTF &= ~(1))

/**
 * Set LED driver pins on Port F as output
 */
#define LED_INIT   (DDRF |= 0x33) //(000110011)

/**
 * take a 16 bit number and shifts it out on the data line.
 */
void shift_out(int word);

/**
 * Initilize hardware
 */
void LED_Init(void);

// mutable state for swiching lights
volatile unsigned int level;
volatile unsigned int color_data[3][4];
void set_data(unsigned int *color);
