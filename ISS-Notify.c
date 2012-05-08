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
volatile unsigned int color_data[3][4];
void set_data(unsigned int *color);

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

int main(void)
{
  int i,j,k;
  
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

  ///unsigned int color[12] = {0,8,128,2048,4,64,1024,2184,2056,136,2176,292};
  
  unsigned int anime_start[8][12] = {{ 273, 273, 273, 273, 273, 273, 273, 273, 273, 273, 273, 273}
	                                  ,{ 546, 546, 546, 546, 546, 546, 546, 546, 546, 546, 546, 546}
	                                  ,{ 819, 819, 819, 819, 819, 819, 819, 819, 819, 819, 819, 819}
	                                  ,{1092,1092,1092,1092,1092,1092,1092,1092,1092,1092,1092,1092}
	                                  ,{1365,1365,1365,1365,1365,1365,1365,1365,1365,1365,1365,1365}
	                                  ,{1638,1638,1638,1638,1638,1638,1638,1638,1638,1638,1638,1638}
	                                  ,{1911,1911,1911,1911,1911,1911,1911,1911,1911,1911,1911,1911}
	                                  ,{2184,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184}};
	
	unsigned int white[12]           = {2184,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184};
	
  unsigned int anime_wheel1[12][12]={{   8,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184}
	                                  ,{  38,   8,2184,2184,2184,2184,2184,2184,2184,2184,2184,2184}
	                                  ,{  68,  38,   8,2184,2184,2184,2184,2184,2184,2184,2184,2184}
	                                  ,{  98,  68,  38,   8,2184,2184,2184,2184,2184,2184,2184,2184}
	                                  ,{ 128,  98,  68,  38,   8,2184,2184,2184,2184,2184,2184,2184}
	                                  ,{ 608, 128,  98,  68,  38,   8,2184,2184,2184,2184,2184,2184}
	                                  ,{1088, 608, 128,  98,  68,  38,   8,2184,2184,2184,2184,2184}
	                                  ,{1568,1088, 608, 128,  98,  68,  38,   8,2184,2184,2184,2184}
	                                  ,{2048,1568,1088, 608, 128,  98,  68,  38,   8,2184,2184,2184}
	                                  ,{1538,2048,1568,1088, 608, 128,  98,  68,  38,   8,2184,2184}
	                                  ,{1028,1538,2048,1568,1088, 608, 128,  98,  68,  38,   8,2184}
	                                  ,{ 518,1028,1538,2048,1568,1088, 608, 128,  98,  68,  38,   8}};
	                            
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
	   
	                               
	unsigned int anime_ran[10][12] =  {{2048,   8,1092,1568,2184,  38,  38,   8,1568,1028,   8,  38}
                                    ,{ 608,   8,  98,1538,1088,2048,1538, 518, 518,  98, 518, 128}
                                    ,{2184,1568,2048,  98,  98,  38,1568, 128,   8, 608,2184,1088}
                                    ,{  98,  68,1538,1088,  98, 128,1568,1538,2048,2048, 608,   8}
                                    ,{1028,   8,1538,1028, 608,1028,1088,1088,   8,2048, 128,   8}
                                    ,{   8, 608,   8,  38,  68,2048, 518,  38,1538,1028,1088,1088}
                                    ,{1088,1088, 608,  68,  98, 608,2048,1092,1568,  98,1092,  98}
                                    ,{  68, 608,   8,1568, 128, 608,1028,1088,  38, 128, 518,2184}
                                    ,{   8,  98,1568,   8, 128,1538,1568,1088,2184, 608,  38,1568}
                                    ,{2184,   8,  38,2048,  68,   8,  38,1568,2048,1028,1092,1092}};

  unsigned int blank[12]           = {0,0,0,0,0,0,0,0,0,0,0,0};
  
  //DDRD &= ~(1<<6);
  //int val = PORTD.0;
  //red[11] = val>>7;
  
  sei();
  
  for (i=0;i<8;i++)
  {
    set_data(anime_start[i]);
    _delay_ms(33);
  }
  
  set_data(white);
  _delay_ms(800);
  
  for (i=0;i<12;i++)
  {
    set_data(anime_wheel1[i]);
    _delay_ms(33);
  }
  
	while (1) {

    for (j=0;j<4;j++)
    {
      for (i=0;i<12;i++)
      {
        set_data(anime_wheel[i]);
        _delay_ms(33);
      }
    }
    
    for (j=0;j<2;j++)
    {
      for (i=0;i<10;i++)
      {
        set_data(anime_ran[i]);
        _delay_ms(33);
      }
      for (i=10;i>=0;i--)
      {
        set_data(anime_ran[i]);
        _delay_ms(33);
      }
    }
    
    set_data(blank);
    _delay_ms(1000);
    
    
	}

  return 0;
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
