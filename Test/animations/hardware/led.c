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
#include <avr/interrupt.h>
#include "led.h"

ISR(TIMER1_COMPA_vect) {
  BLANK_HIGH;
  if (level == 3) {
    shift_out(color_data[2][level]);
    shift_out(color_data[1][level]);
    shift_out(color_data[0][level]);

    OCR1A = 50;
    level--;
  }
  else if (level == 2) {
    shift_out(color_data[2][level]);
    shift_out(color_data[1][level]);
    shift_out(color_data[0][level]);

    OCR1A = 25;
    level--;
  }
  else if (level == 1) {
    shift_out(color_data[2][level]);
    shift_out(color_data[1][level]);
    shift_out(color_data[0][level]);
    
    OCR1A = 15;
    level--;
  }
  else if (level == 0) {
    shift_out(color_data[2][level]);
    shift_out(color_data[1][level]);
    shift_out(color_data[0][level]);

    OCR1A = 8;
    level = 3;
  }
  BLANK_LOW;
}

void LED_Init(void)
{
  // LED Driver init
	LED_INIT;
	SIN_LOW;
	SCLK_LOW;
	LATCH_LOW;
	//BLANK_HIGH;
	BLANK_LOW;
	
	level = 0;
	
	/* Timer Initialization */
  TIMSK1 = _BV(OCIE1A);
	TCCR1B |=
    // Prescaler divides the clock rate by 256.
    (_BV(CS12) )  |
    // Set WGM12 bit to clear timer on compare with the OCR1A
    // register.
    _BV(WGM12);
    
  // 8 MHz clock. 1024 prescaler. Counting to 7812 is one second.
  OCR1A = 1;
}

void set_data(unsigned int *color)
{
  int i;
  int j;
  
  unsigned int red[12], green[12], blue[12];
  
  for (i=0;i<12;i++)
  {
    red[i]   =  color[i] & 0x00F;
    green[i] = (color[i] & 0x0F0) >> 4;
    blue[i]  = (color[i] & 0xF00) >> 8;
  }
  
  //unsigned int *colors[3] = {red,green,blue};
  
  unsigned int *colors[3] = {red,green,blue};
  
  // Reset
  for (j=0;j<4;j++)
  {
    color_data[2][j] = 0;
    color_data[1][j] = 0;
    color_data[0][j] = 0;
  }
  
  for (j=0;j<3;j++)
  {
    for (i=0;i<12;i++)
    {
      switch (colors[j][i])
      {
        case 1:
          color_data[j][0] |= 1<<i;
          break;
        case 2:
          color_data[j][1] |= 1<<i;
          break;
        case 3:
          color_data[j][2] |= 1<<i;
          break;
        case 4:
          color_data[j][2] |= 1<<i;
          color_data[j][1] |= 1<<i;
          break;
        case 5:
          color_data[j][3] |= 1<<i;
          break;
        case 6:
          color_data[j][3] |= 1<<i;
          color_data[j][1] |= 1<<i;
          break;
       case 7:
          color_data[j][3] |= 1<<i;
          color_data[j][2] |= 1<<i;
          color_data[j][1] |= 1<<i;
          break;
       case 8:
          color_data[j][3] |= 1<<i;
          color_data[j][2] |= 1<<i;
          color_data[j][1] |= 1<<i;
          color_data[j][0] |= 1<<i;
          break;
      } // END switch
    } // END for i
  } // END for j
}

void shift_out(int word)
{
  int i = 0;
  LATCH_HIGH;
  LATCH_LOW;

  // highest 4 bits always 0
  SIN_LOW;
  SCLK_HIGH;
  SCLK_LOW;   // one
  SCLK_HIGH;
  SCLK_LOW;   // two
  SCLK_HIGH;
  SCLK_LOW;   // three
  //SCLK_HIGH;
  //SCLK_LOW;   // four
  
  //SIN_HIGH;
  for (i=12;i>=0;i--)
  {
    SIN_LOW;
    
    // Bit
    if ((word & (1 << i)) != 0)
      SIN_HIGH;
    
    // Clock
    SCLK_HIGH;
    SCLK_LOW;
  }

  LATCH_HIGH;
  LATCH_LOW;  
}
