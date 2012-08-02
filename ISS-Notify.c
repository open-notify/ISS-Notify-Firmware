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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "library/LUFA/Version.h"
#include "library/LUFA/Drivers/USB/USB.h"
#include "hardware/led.h"
#include "hardware/analog.h"
#include "hardware/rtc.h"
#include "hardware/charge.h"
#include "Descriptors.h"


// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

// Hardware initializer routine
void Setup_Hardware(void);

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


int main(void)
{
  uint8_t batv;
  int j,k;
  
  uint32_t timelapse;

	Setup_Hardware();
	
	unsigned int battery_wheel[12] = {8,  8,  24, 40, 39, 55, 70, 99, 131, 130, 129, 128};

  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	
  // Enable interupts
  sei();

  batv = get_battery_voltage();
  unsigned int show[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  for (j=0;j<=batv;j++)
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
  
  while(1) {
  
    /*
    if (timelapse > 100000)
    {
      timelapse = 0;
      
      uint32_t time = get_time();
      char printbuf[32];
		  int len = sprintf(printbuf, "s: %lu \n", time);
      fputs(printbuf, &USBSerialStream);
    }
    //fputs("message\n", &USBSerialStream);
    //CDC_Device_SendString(&VirtualSerial_CDC_Interface, "message\n");
    */
    
		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		//CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		/*
    uint8_t ColourUpdate = fgetc(&USBSerialStream);
    
    if (ColourUpdate == 97) {
      unsigned int col = fgetc(&USBSerialStream);
      unsigned int show[12] = {col,col,col,col,col,col,col,col,col,col,col,col};
      set_data(show);
    }
    */
    int n;
    int read = fscanf (&USBSerialStream,"c%d",&n);
    if (read > 0) {
      unsigned int show[12] = {n,n,n,n,n,n,n,n,n,n,n,n};
      set_data(show);
    }
    
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
		
		timelapse++;
		;
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
	
	unsigned int show[12] = {0,0,0,0,0,0,0,0,0,2048,0,0};
  set_data(show);

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
	
	// Bat measure
	DDRB |= 0x01;
  
  /*
	TIMSK3 = _BV(OCIE3A);
	TCCR3B |=
    // Prescaler divides the clock rate by 256.
    (_BV(CS32) )  |
    // Set WGM12 bit to clear timer on compare with the OCR1A
    // register.
    _BV(WGM32);
    
  // 8 MHz clock. 1024 prescaler. Counting to 7812 is one second.
  OCR3A = 100;
  */

}
