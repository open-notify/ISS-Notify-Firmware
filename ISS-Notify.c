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
#include <stdint.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "library/usb_serial.h"
#include "led.h"
#include "analog.h"
#include "usb.h"
#include "rtc.h"

// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

// Battery stuff
///TODO: Fix values
#define MAX_BAT_ADC 390
#define MIN_BAT_ADC 280

// The battery voltage can be read off of pin PB6
// The mux value for the ATMEGA32U4 is 0x25
#define BAT_SENSE 0x25

// Hardware initializer routine
void Setup_Hardware(void);

// Measure battery voltage
int get_battery_voltage(void);

int main(void)
{
  int i,j,k;
  
	Setup_Hardware();
	
	/*
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
	
  */
	unsigned int battery_wheel[12] = {8,  8,  24, 40, 39, 55, 70, 99, 131, 130, 129, 128};

  // Enable interupts
  sei();

  i = get_battery_voltage();
  unsigned int show[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  for (j=0;j<=i;j++)
  {
     for (k=0;k<=j;k++)
       show[k] = battery_wheel[k];
     set_data(show);
     _delay_ms(8);
  }
  _delay_ms(400);
  
  for (j=11;j>=0;j--)
  {
     show[j] = 0;
     set_data(show);
     _delay_ms(8);
  }
  
	while (1) {
		// wait for the user to run their terminal emulator program
		// which sets DTR to indicate it is ready to receive.
		while (!(usb_serial_get_control() & USB_SERIAL_DTR)) /* wait */ ;

		// discard anything that was received prior.  Sometimes the
		// operating system or other software will send a modem
		// "AT command", which can still be buffered.
		usb_serial_flush_input();

		// print a nice welcome message
		send_str(PSTR("Begin\n"));
		
		/*
    time testt;
    
    testt.year   = 12;
    testt.month  = 07;
    testt.day    = 21;
    testt.hour   = 18;
    testt.minute = 30;
    testt.second = 55;
    
    //reset_rtc();
    //set_time(testt);
    */
    
		while (1) {
		  
		  send_str(PSTR("clock talk\n"));
		  
		  uint32_t time = get_time();
		  
		  //uint8_t setup = read_address(0x03);
		  
		  char printbuf[32];
		  int len = sprintf(printbuf, "s: %lu", time);
		  int l;
		  for (l=0;l<len;l++){
		    usb_serial_putchar(printbuf[l]);
		  }
		  send_str(PSTR("\n"));
		  
		  _delay_ms(10000);
		  
    }
	}

  return 0;
}

int get_battery_voltage(void)
{
    // Enable battery measurment
    PORTB |=  1;
    
    // Read voltage on battery sense pin
	  int val = adc_read(BAT_SENSE);
	  
	  // 0 isn't 0 V, it's the miniumum battery volage through voltage divider
	  if (val >= MIN_BAT_ADC)
	    val = val - MIN_BAT_ADC;
	  else
	    return 0;  // no/dead battery, we're done
	  
	  // Scale between 0 and 11
	  val *= 11;
	  val /= (MAX_BAT_ADC - MIN_BAT_ADC);
	  
	  // Clamp to 11;
	  if (val > 11)
	    val = 11;
	  
	  // Disable battery measurment
    PORTB &=  ~(1);
	  
	  return val;
}

void Setup_Hardware(void)
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
	
	// USB
	usb_init();
	while (!usb_configured()) /* wait */ ;
	//_delay_ms(500);
	
	// ready clock
	rtc_init();
	
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
