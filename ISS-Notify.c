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
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "library/LUFA/Version.h"
#include "library/LUFA/Drivers/USB/USB.h"
#include "hardware/led.h"
#include "hardware/analog.h"
#include "hardware/rtc.h"
#include "hardware/charge.h"
#include "structs.h"
#include "Descriptors.h"


// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

// Hardware initializer routine
void Setup_Hardware(void);
uint32_t millis(void);

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream;

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber         = 0,

				.DataINEndpointNumber           = CDC_TX_EPNUM,
				.DataINEndpointSize             = CDC_TXRX_EPSIZE,
				.DataINEndpointDoubleBank       = false,

				.DataOUTEndpointNumber          = CDC_RX_EPNUM,
				.DataOUTEndpointSize            = CDC_TXRX_EPSIZE,
				.DataOUTEndpointDoubleBank      = false,

				.NotificationEndpointNumber     = CDC_NOTIFICATION_EPNUM,
				.NotificationEndpointSize       = CDC_NOTIFICATION_EPSIZE,
				.NotificationEndpointDoubleBank = false,
			},
	};

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

volatile uint8_t reset_alarm = 0;
volatile uint8_t alarm       = 0;
volatile uint8_t pass        = 0;
volatile uint32_t ms         = 0;

// EEPROM Storage
uint16_t EEMEM NonVolatileColor;
ipass EEMEM ISS_Passes[10];

unsigned int show[12] = {0,0,0,0,0,0,0,0,0,0,0,0};


/**
 * Wake up on alarm
 */
ISR(INT3_vect) 
{
  // Disable interupt
  EIMSK &= ~(1<<INT3);
  
  //unsigned int show[12] = {0,8,0,8,0,128,0,8,0,8,0,8};
  //set_data(show);
  //reset_alarm = 1;
  alarm = 1;
}

// milli timer
ISR(TIMER3_COMPA_vect)
{
  ms++;
} 

// milli get
uint32_t millis(void) 
{ 
  return ms; 
}

int main(void)
{
  uint8_t batv;
  int i,j,k,a;
  uint32_t premills = 0;

	Setup_Hardware();
   
	unsigned int battery_wheel[12] = {8,  8,  24, 40, 39, 55, 70, 99, 131, 130, 129, 128};

  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	
  // Enable interupts
  sei();

  //batv = get_battery_voltage();
    //for (j=0;j<=batv;j++)
  //{
  //   for (k=0;k<=j;k++)
  //     show[k] = battery_wheel[k];
  //   set_data(show);
  //   _delay_ms(8);
  //}
  //_delay_ms(400);
  
  //for (j=11;j>=0;j--)
  //{
  //   show[j] = 0;
  //   set_data(show);
  //   _delay_ms(8);
  //}
  
  //for (i=0;i<12;i++) {
  // show[i] = 0;
  //}

    uint16_t stored_color;

    stored_color = eeprom_read_word(&NonVolatileColor);
    for (i=0;i<12;i++) {
        show[i] = (int) stored_color;
        set_data(show);
        _delay_ms(20);
    }
    _delay_ms(100);
    for (i=11;i>=0;i--) {
        show[i] = 0;
        set_data(show);
        _delay_ms(20);
    }

  a = 0;
  while(1) {
 
    // Color update
    int n;
    int read = fscanf(&USBSerialStream,"c%d",&n);
    if (read > 0) {
      for (i=0;i<12;i++) {
        show[i] = n;
      }
      set_data(show);
    }
    
    // knock knock
    char hello;
    if (fscanf(&USBSerialStream,"hello%c", &hello) == 1) {
      // whos there?
      fputs("hi", &USBSerialStream);
    }
    
    if (fscanf(&USBSerialStream,"ms%c", &hello) == 1) {
      // whos there?
      fprintf(&USBSerialStream, "%ul", millis());
    }

    // Set color
    int color;
    if (fscanf(&USBSerialStream,"setc%d", &color) == 1) {
         eeprom_update_word((uint16_t*) &NonVolatileColor, (uint16_t)color);  
    }

    // Set block
    int tvalue;
    ipass block[10];
    if (fscanf(&USBSerialStream, "z%d", &tvalue) == 1) {
        block[0].time = 1234567;
        block[0].duration = 500;
        eeprom_update_block((const void * ) &block, (void*) &ISS_Passes, sizeof(ipass));        
    }

    // Get block
    if (fscanf(&USBSerialStream,"getvalue%c", &hello) == 1) {
        eeprom_read_block((void*) &block, (const void *) &ISS_Passes, sizeof(ipass));
        fprintf(&USBSerialStream, "%lu - %u", block[0].time, block[0].duration);
    }

    // get time
    char readtime;
    if (fscanf(&USBSerialStream,"t%c", &readtime) == 1) {
      time now = get_time();
      fprintf(&USBSerialStream, "Y%02dM%02dD%02dH%02dM%02dS%02d", now.year, now.month, now.day, now.hour, now.minute, now.second);
    }
    
    // set time
    int year, month, day, hour, min, sec;
    if (fscanf(&USBSerialStream,"TY%dM%dD%dH%dM%dS%d", &year, &month, &day, &hour, &min, &sec) == 6) {
      time t = mktime((uint8_t) year, (uint8_t) month, (uint8_t) day, (uint8_t) hour, (uint8_t) min, (uint8_t) sec);
      set_time(t);
      fputs("set", &USBSerialStream);
    }
    
    // set alarm
    if (fscanf(&USBSerialStream,"AY%dM%dD%dH%dM%dS%d", &year, &month, &day, &hour, &min, &sec) == 6) {
      time t = mktime((uint8_t) year, (uint8_t) month, (uint8_t) day, (uint8_t) hour, (uint8_t) min, (uint8_t) sec);
      set_alarm0(t);
      fputs("set", &USBSerialStream);
    }
    
    char ra;
    if (fscanf(&USBSerialStream,"ra%c", &ra) == 1) {
      // Echo
      clear_alarm0();
      fputs("reset alarm", &USBSerialStream);
    }

    // battery
    char bat;
    if (fscanf(&USBSerialStream,"bat%c", &bat) == 1) {
      // get charge status
      CHARGE_STATUS chargestat = get_charge_status();

      if (get_power_status() < 0)
      {
        fputs("bNOBAT", &USBSerialStream);
      }
      else
      { 
        switch (chargestat) {
          case BULK_CHARGE:
            fputs("bCHARGING", &USBSerialStream);
            break;
          case TRICKLE_CHARGE:
            fputs("bCHARGED", &USBSerialStream);
            break;
          default:
            fputs("bSTANDBY", &USBSerialStream);
            break;
        }
      }
    }
    
    if (pass > 0)
    {
      uint32_t curmills = millis();
      if((curmills - premills) > 100) {
        show[a] = 8;
        set_data(show);
        a++;
        if (a >= 12)
        {
          for (i=0;i<12;i++) {
           show[i] = 0;
          }
          a = 0;
        }
        premills = curmills;
      }
    }
    
    // When an alarm is triggered by the RTC:
    if (alarm > 0)
    {
      // Turn off alarm
      clear_alarm0();
      
      pass = 1;
      
      //unsigned int beep[12] = {3840,8,0,8,0,128,0,8,0,8,1954,8};
      //set_data(beep);
    
      // Renenable alarm interupt
		  EIMSK |=  (1<<INT3);
      alarm = 0;
    }
    
    // USB Service
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
		
		if (reset_alarm > 0)
		  clear_alarm0();
		  reset_alarm = 0;
		  // Renenable interupts
		  EIMSK |=  (1<<INT3);
  }
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
  //LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
  //unsigned int show[12] = {0,0,0,0,0,8,0,0,0,0,0,0};
  //set_data(show);
	return;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
  //LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
  //unsigned int show[12] = {0,0,0,0,0,8,0,0,0,0,0,0};
  //set_data(show);
	return;
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
	
    //unsigned int show[12] = {0,0,0,0,0,0,0,0,0,2048,0,0};
    //set_data(show);

	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
	
	//unsigned int show[12] = {0,0,0,0,0,0,0,0,0,2048,0,0};
  //set_data(show);
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

	USB_Init();

	RTC_Init();
	
	charge_Init();

  // milli timer
  OCR3AL = 8;
  TCCR3B = (1<<WGM32)|(5<<CS30); //ctc, div1024
  TIMSK3 = (1<<OCIE3A); //compa irq
}
