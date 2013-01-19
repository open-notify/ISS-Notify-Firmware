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

void ack(void);
void say_ms(void);
void set_color(void);
void say_time(void);

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
//uint8_t EEMEM NonVolatileOptions;
uint8_t EEMEM NumOfStoredISSPasses;
ipass EEMEM StoredISSPasses[50];

// Global variable that always stores current color
unsigned int show[12] = {0,0,0,0,0,0,0,0,0,0,0,0};


// Commands:
static const cmd COMMANDS[4] = {  {c: 'a', resp: ack},
								  {c: 'm', resp: say_ms},
								  {c: 'c', resp: set_color},
								  {c: 't', resp: say_time},
							   };


/*******************************************************************************
* INTERRUPTS
*******************************************************************************/

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

/**
 * Counting milliseconds
 */
ISR(TIMER3_COMPA_vect)
{
	ms++;
}


/*******************************************************************************
* MAIN
*******************************************************************************/
int main(void)
{
	int i;

	// Setup hardware
	Setup_Hardware();

	//unsigned int battery_wheel[12] = {8,  8,  24, 40, 39, 55, 70, 99, 131, 130, 129, 128};

	/**
	 * Setting up USB communication
	 * Create a regular character stream for the interface so that 
	 * it can be used with the stdio.h functions
	 */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);


	/**
	 * TODO: Sanity Check
	 */
	// Reset alarms
	// What time is it
	// Are we inside a pass?
	//     true: show pass
	//     false: set alarm for next pass


	// Enable interupts
	sei();


	// Wake up animation:
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


	/***************************************************************************
	* THE MAIN LOOP
	***************************************************************************/
	while(1) {

		// Listen for commands over USB serial
		char c;
		if (fscanf(&USBSerialStream,"ISS%c\n", &c) == 1) {
			for (i=0;i<4;i++) {
				cmd command = COMMANDS[i];
				if (command.c == c) {
					command.resp();
					break;
				}
			}
		}

		/*
		// Color update
		int n;
		int read = fscanf(&USBSerialStream,"c%d",&n);
		if (read > 0) {
			for (i=0;i<12;i++) {
				show[i] = n;
			}
		set_data(show);
		}

    // Set block
    int npasses;
    ipass block[10];
    if (fscanf(&USBSerialStream, "z%d\n", &npasses) == 1) {
        
        for (i=0;i<npasses;i++) {
            uint32_t t;
            uint16_t d;
            ipass p;
            
            fscanf(&USBSerialStream, "%lu,%u,", &t, &d);
            p.time = t;
            p.duration = d;
            block[i] = p;
        }

        eeprom_update_block((const void * ) &block, (void*) &ISS_Passes, sizeof(ipass)*npasses);        
    }

    // Get block
    if (fscanf(&USBSerialStream,"getvalue%c", &hello) == 1) {
        eeprom_read_block((void*) &block, (const void *) &ISS_Passes, sizeof(ipass)*10);
        for (i=0;i<10;i++) {
            fprintf(&USBSerialStream, "%lu - %u\n", block[i].time, block[i].duration);
        }
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
	*/
    
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

/**
 * Commands:
 */
void ack(void)
{
	fputs("ack", &USBSerialStream);
}

void say_ms(void)
{
	fprintf(&USBSerialStream, "%lu", millis());
}

void set_color(void)
{
	int color;
	if (fscanf(&USBSerialStream,"%d", &color) == 1) {
		eeprom_update_word((uint16_t*) &NonVolatileColor, (uint16_t)color);
		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}

void say_time(void)
{
	time now = get_time();
	fprintf(&USBSerialStream, "Y%02dM%02dD%02dH%02dM%02dS%02d", now.year, now.month, now.day, now.hour, now.minute, now.second);
}

/**
 * Helper funtion to get current number of milliseconds since boot
 * ms is volitile and updated in a timer
 */
uint32_t millis(void) { return ms; }


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

/**
 * Hardware set up. Most things are done in library code.
 */
void Setup_Hardware(void)
{
	// set for 8 MHz clock (see Makefile)
	CPU_PRESCALE(0);

	// JTAG Disable
	MCUCR = (1 << JTD); 
	MCUCR = (1 << JTD); 

	// Disable watchdog if enabled by bootloader/fuses
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// Disable Interupts
	cli();

	// Initilise pins and interupts for LED driver
	LED_Init();

	// Initilise USB
	USB_Init();

	// Initilise pins and interupts for talking to RTC
	RTC_Init();

	// Ininilise pins for sensing battary volatge and charge state
	charge_Init();

	// set up millisecond timer
	OCR3AL = 8;
	TCCR3B = (1<<WGM32)|(5<<CS30); //ctc, div1024
	TIMSK3 = (1<<OCIE3A); //compa irq
}
