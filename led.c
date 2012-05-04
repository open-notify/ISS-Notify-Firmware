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
#include "led.h"

void shift_out(int word)
{
  int i = 0;
  LATCH_HIGH;
  LATCH_LOW;
  
  //SIN_HIGH;
  for (i=0;i<16;i++)
  {
    // Bit
    if ((word & (1 << i)) != 0)
      SIN_HIGH;
    else
      SIN_LOW;
    
    // Clock
    SCLK_HIGH;
    SCLK_LOW;
  }
 // SIN_LOW;

  LATCH_HIGH;
  LATCH_LOW;  
}
