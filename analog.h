/**
 * @file
 * @author  Nathan Bergey <nathan.bergey@gmail.com>
 * @version Thursday, May 26 2012
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
 * @brief ADC reading utility
 *
 * @section DESCRIPTION
 *
 * This is the header file for ADC reading on the ISS-Notify v0.1 board. It 
 * defines the bat sense pin.
 */

// Analog prescaler
#define ADC_PRESCALER ((1<<ADPS2) | (1<<ADPS0))

// functions
int16_t adc_read(uint8_t mux);
void analogReference(uint8_t mode);
