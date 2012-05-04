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

// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

// mutable state for swiching lights
volatile unsigned int level = 0;
volatile unsigned int red_data[6];
volatile unsigned int green_data[6];
volatile unsigned int blue_data[6];

void set_data(unsigned int *red, unsigned int *green, unsigned int *blue);

ISR(TIMER1_COMPA_vect) {
  BLANK_HIGH;
  if (level == 0) {
    
    shift_out(blue_data[level]);
    shift_out(green_data[level]);
    shift_out(red_data[level]);

    OCR1A = 120;
    level++;
  }
  else if (level == 1) {

    shift_out(blue_data[level]);
    shift_out(green_data[level]);
    shift_out(red_data[level]);

    OCR1A = 80;
    level++;
  }
  else if (level == 2) {

    shift_out(blue_data[level]);
    shift_out(green_data[level]);
    shift_out(red_data[level]);
    
    OCR1A = 50;
    level++;
  }
  else if (level == 3) {
    shift_out(blue_data[level]);
    shift_out(green_data[level]);
    shift_out(red_data[level]);
    
    OCR1A = 30;
    level++;
  }
  else if (level >= 4) {
    shift_out(blue_data[level]);
    shift_out(green_data[level]);
    shift_out(red_data[level]);
    
    OCR1A = 20;
    level = 0;
  }

  BLANK_LOW;
}

int main(void)
{
  int i;
  
	// set for 8 MHz clock (see Makefile)
	CPU_PRESCALE(0);
	
  // JTAG Disable
  MCUCR = (1 << JTD); 
	MCUCR = (1 << JTD); 
	
	// LED Driver init
	LED_INIT;
	SIN_LOW;
	SCLK_LOW;
	LATCH_LOW;
	BLANK_HIGH;
  
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

	sei();
	
	unsigned int red_anime[12][12] =   { {4,3,1,0,0,0,0,0,1,3,4,5}  // 1
	                                    ,{3,1,0,0,0,0,0,1,3,4,5,4}  // 2
	                                    ,{1,0,0,0,0,0,1,3,4,5,4,3}  // 3
	                                    ,{0,0,0,0,0,1,3,4,5,4,3,1}  // 4
	                                    ,{0,0,0,0,1,3,4,5,4,3,1,0}  // 5
	                                    ,{0,0,0,1,3,4,5,4,3,1,0,0}  // 6
	                                    ,{0,0,1,3,4,5,4,3,1,0,0,0}  // 7
	                                    ,{0,1,3,4,5,4,3,1,0,0,0,0}  // 8
	                                    ,{1,3,4,5,4,3,1,0,0,0,0,0}  // 9
	                                    ,{3,4,5,4,3,1,0,0,0,0,0,1}  //10
	                                    ,{4,5,4,3,1,0,0,0,0,0,1,3}  //11
	                                    ,{5,4,3,1,0,0,0,0,0,1,3,4}};//12
	
  unsigned int green_anime[12][12] = { {0,0,0,0,1,3,4,5,4,3,1,0}  // 5
	                                    ,{0,0,0,1,3,4,5,4,3,1,0,0}  // 6
	                                    ,{0,0,1,3,4,5,4,3,1,0,0,0}  // 7
	                                    ,{0,1,3,4,5,4,3,1,0,0,0,0}  // 8
	                                    ,{1,3,4,5,4,3,1,0,0,0,0,0}  // 9
	                                    ,{3,4,5,4,3,1,0,0,0,0,0,1}  //10
	                                    ,{4,5,4,3,1,0,0,0,0,0,1,3}  //11
	                                    ,{5,4,3,1,0,0,0,0,0,1,3,4}  //12
	                                    ,{4,3,1,0,0,0,0,0,1,3,4,5}  // 1
	                                    ,{3,1,0,0,0,0,0,1,3,4,5,4}  // 2
	                                    ,{1,0,0,0,0,0,1,3,4,5,4,3}  // 3
	                                    ,{0,0,0,0,0,1,3,4,5,4,3,1}};// 4
	
	unsigned int blue_anime[12][12] =  { {1,3,4,5,4,3,1,0,0,0,0,0}  // 9
	                                    ,{3,4,5,4,3,1,0,0,0,0,0,1}  //10
	                                    ,{4,5,4,3,1,0,0,0,0,0,1,3}  //11
	                                    ,{5,4,3,1,0,0,0,0,0,1,3,4}  //12
	                                    ,{4,3,1,0,0,0,0,0,1,3,4,5}  // 1
	                                    ,{3,1,0,0,0,0,0,1,3,4,5,4}  // 2
	                                    ,{1,0,0,0,0,0,1,3,4,5,4,3}  // 3
	                                    ,{0,0,0,0,0,1,3,4,5,4,3,1}  // 4
	                                    ,{0,0,0,0,1,3,4,5,4,3,1,0}  // 5
	                                    ,{0,0,0,1,3,4,5,4,3,1,0,0}  // 6
	                                    ,{0,0,1,3,4,5,4,3,1,0,0,0}  // 7
	                                    ,{0,1,3,4,5,4,3,1,0,0,0,0}};// 8
	                                    
	while (1) {
    for (i=0;i<12;i++) {
      set_data(red_anime[i], green_anime[i], blue_anime[i]);
      _delay_ms(35);
    }
	}

  return 0;
}

void set_data(unsigned int *red, unsigned int *green, unsigned int *blue)
{
  int i;
  int j;
  
  // Reset
  for (j=0;j<6;j++)
  {
    red_data[j]   = 0;
    green_data[j] = 0;
    blue_data[j]  = 0;
  }
  
  for (i=0;i<13;i++)
  {
    for (j=0;j<6;j++)
    {
      if (red[i] == j)
        red_data[5-j] |= (1<<(i+4));
      if (green[i] == j)
        green_data[5-j] |= (1<<(i+4));
      if (blue[i] == j)
        blue_data[5-j] |= (1<<(i+4));
    }
  }
}
