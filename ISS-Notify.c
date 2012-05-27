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
 * @brief ISS-Notify Firmware
 *
 * @section DESCRIPTION
 *
 * The main file of the firmware that runs ISS-Notify v0.1
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "led.h"
#include "analog.h"

// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

// Hardware initializer routine
void Setup_Hardware();

// Measure battery voltage
int get_battery_voltage();

int main(void)
{
  int i,j,k;
  
	Setup_Hardware();
	
  unsigned int anime_wheel[12][12] ={{   8, 518,1028,1538,2048,1568,1088, 608, 128,  98,  68,  38}
	                                  ,{  38,   8, 518,1028,1538,2048,1568,1088, 608, 128,  98,  68}
	                                  ,{  68,  38,   8, 518,1028,1538,2048,1568,1088, 608, 128,  98}
	                                  ,{  98,  68,  38,   8, 518,1028,1538,2048,1568,1088, 608, 128}
	                                  ,{ 128,  98,  68,  38,   8, 518,1028,1538,2048,1568,1088, 608}
	                                  ,{ 608, 128,  98,  68,  38,   8, 518,1028,1538,2048,1568,1088}
	                                  ,{1088, 608, 128,  98,  68,  38,   8, 518,1028,1538,2048,1568}
	                                  ,{1568,1088, 608, 128,  98,  68,  38,   8, 518,1028,1538,2048}
	                                  ,{2048,1568,1088, 608, 128,  98,  68,  38,   8, 518,1028,1538}
	                                  ,{1538,2048,1568,1088, 608, 128,  98,  68,  38,   8, 518,1028}
	                                  ,{1028,1538,2048,1568,1088, 608, 128,  98,  68,  38,   8, 518}
	                                  ,{ 518,1028,1538,2048,1568,1088, 608, 128,  98,  68,  38,   8}};

  // Enable interupts
  sei();
  
	while (1) {
	  i = get_battery_voltage();
	  // Spin wheel depending on value
	  set_data(anime_wheel[i]);
	  _delay_ms(100);
  }

  return 0;
}

int get_battery_voltage()
{
    // Enable battery measurment
    PORTB |=  1;
    
    // Read voltage on battery sense pin
	  int val = adc_read(BAT_SENSE);
	  // Scale between 0 and 11
	  val /= 90;
	  
	  // Disable battery measurment
    PORTB &=  ~(1);
	  
	  return val;
}

void Setup_Hardware()
{
  // set for 8 MHz clock (see Makefile)
	CPU_PRESCALE(0);
	
  // JTAG Disable
  MCUCR = (1 << JTD); 
	MCUCR = (1 << JTD); 
	
	level = 0;
	
	// LED Driver init
	LED_INIT;
	SIN_LOW;
	SCLK_LOW;
	LATCH_LOW;
	BLANK_HIGH;
	
	// Bat measure
	DDRB |= 0x01;
  
  
  cli();
  
	TIMSK1 = _BV(OCIE1A);
	TCCR1B |=
    // Prescaler divides the clock rate by 256.
    (_BV(CS12) )  |
    // Set WGM12 bit to clear timer on compare with the OCR1A
    // register.
    _BV(WGM12);
    
  // 8 MHz clock. 1024 prescaler. Counting to 7812 is one second.
  OCR1A = 1;
  
  BLANK_LOW;
}
