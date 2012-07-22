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
#include <string.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
//#include "library/usb_serial.h"
#include "led.h"
#include "analog.h"
//#include "usb.h"
#include "rtc.h"

#include "Descriptors.h"

#include "library/LUFA/Version.h"
#include "library/LUFA/Drivers/USB/USB.h"

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

		
// Measure battery voltage
int get_battery_voltage(void);

int main(void)
{
  int i,j,k;
  
	Setup_Hardware();
	
	unsigned int battery_wheel[12] = {8,  8,  24, 40, 39, 55, 70, 99, 131, 130, 129, 128};

  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
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
  
  while(1) {
  
    CDC_Device_SendString(&VirtualSerial_CDC_Interface, "messages\n");

		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
		
		//_delay_ms(1000);
		;
  }
  /*
	while (1) {
		// wait for the user to run their terminal emulator program
		// which sets DTR to indicate it is ready to receive.
		while (!(usb_serial_get_control() & USB_SERIAL_DTR)) ; //wait

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
	*/
}

/** Event handler for the library USB Connection event. */

void EVENT_USB_Device_Connect(void)
{
  //LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
	return;
}


/** Event handler for the library USB Disconnection event. */

void EVENT_USB_Device_Disconnect(void)
{
  //LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	return;
}


/** Event handler for the library USB Configuration Changed event. */

void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}


/** Event handler for the library USB Control Request reception event. */

void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
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
	
	
	// LUFA USB
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	//clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	USB_Init();

	/* Timer Initialization */
//	OCR0A  = 100;
//	TCCR0A = (1 << WGM01);
//	TCCR0B = (1 << CS00);
//	TIMSK0 = (1 << OCIE0A);
	
	// END LUFA USB
	
	
	/*
	// USB
	usb_init();
	while (!usb_configured()) ; // wait
	//_delay_ms(500);
	*/
	
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
