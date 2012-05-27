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
 * This reads ADC values the ISS-Notify v0.1 board. Given a mux value that 
 * corrisponds to a pin on the ATMEGA32U4 it will return the ADC of that pin.
 * This is designed to be called once, for sampling a voltage rarely.
 */

#include <avr/io.h>
#include "analog.h"

void analogReference(uint8_t mode)
{
	aref = mode & 0xC0;
}

int16_t adc_read(uint8_t mux)
{
    uint8_t low;

    ADCSRA = (1<<ADEN) | ADC_PRESCALER;             // enable ADC
    ADCSRB = (1<<ADHSM) | (mux & 0x20);             // high speed mode
    ADMUX = aref | (mux & 0x1F);                    // configure mux input
    ADCSRA = (1<<ADEN) | ADC_PRESCALER | (1<<ADSC); // start the conversion
    while (ADCSRA & (1<<ADSC)) ;                    // wait for result
    low = ADCL;                                     // must read LSB first
    return (ADCH << 8) | low;                       // must read MSB only once!
}

