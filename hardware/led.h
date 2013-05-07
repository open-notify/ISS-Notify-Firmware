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
 * @brief LED Driver for the TLC5971
 *
 * @section DESCRIPTION
 *
 * This is the header file for the LED drivers on the ISS-Notify v0.1 board. It
 * sets up the pins.
 */

/* Data in line (Port F1)*/
#define SIN_HIGH    (PORTF |=  (1<<1))
#define SIN_LOW     (PORTF &= ~(1<<1))

/* Clock line (Port F0)*/
#define SCLK_HIGH   (PORTF |=  (1))
#define SCLK_LOW    (PORTF &= ~(1))

/* Set LED driver pins on Port F as output */
#define LED_INIT    (DDRF |= 0x03) //(000000011)

/* TLC5971 Codes */
#define TLC_WRITE   0x25 //Write command
#define TLC_OUTTMG  0x10 //When this bit is '1', OUTXn are turned on or off at the rising edge of the selected GS reference clock.
#define TLC_EXTGCK  0x08 //When this bit is '1', PWM timing refers to the SCKI clock.
#define TLC_TMGRST  0x04 //When this bit is '1', the GS counter is reset to '0' and all constant-current outputs are forced off when the internal latch pulse is generated for data latching.
#define TLC_DSPRPT  0x02 //Auto display repeat mode enable
#define TLC_BLANK   0x01 //Constant-current output enable


/**
 * Initilize hardware
 */
void LED_Init(void);
void led(uint16_t (*lights)[3]);
