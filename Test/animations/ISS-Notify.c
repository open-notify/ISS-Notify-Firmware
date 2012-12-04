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
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include "hardware/led.h"

// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

// Hardware initializer routine
void Setup_Hardware(void);

// Animations
int show[12]        = {0,0,0,0,0,0,0,0,0,0,0,0};
int colorwheel[12]  = {   8, 518,1028,1538,2048,1568,1088, 608, 128,  98,  68,  38};
int pong[12]        = {4095,546,273,0,0,0,0,0,0,0,0,0};
int ping[12]        = {4095,0,0,0,0,0,0,0,0,0,273,546};

int main(void)
{
  int i,j,k;
  
	Setup_Hardware();
	
  // Enable interupts
  sei();

  while (1)
  {
    // bring on wheel
    for (i=0;i<12;i++)
    {
      show[i] = colorwheel[i];
      set_data(show);
      _delay_ms(40);
    }
    
    // spin wheel
    for (k=0;k<4;k++)
    {
      for (i=11;i>=0;i--)
      {
        for (j=0;j<12;j++)
        {
          show[j] = colorwheel[(i+j) % 12];
        }
        set_data(show);
        _delay_ms(40);
      }
    }
    
    // red pong
    for (i=0;i<12;i++)
    {
      for (j=0;j<12;j++)
      {
        show[j] = pong[(i+j) % 12];
      }
      set_data(show);
      _delay_ms(40);
    }
    // red ping
    for (i=11;i>=0;i--)
    {
      for (j=0;j<12;j++)
      {
        show[j] = ping[(i+j) % 12];
      }
      set_data(show);
      _delay_ms(40);
    }
    
    // Quiet
    for (i=0;i<12;i++)
    {
      show[i] = 0;
    }
    set_data(show);
    _delay_ms(12000);
    
  } // end loop
}

void Setup_Hardware(void)
{
  // set for 8 MHz clock (see Makefile)
	CPU_PRESCALE(0);
	
  // JTAG Disable
  MCUCR = (1 << JTD); 
	MCUCR = (1 << JTD); 
	
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	
	cli();
	
  LED_Init();
}
