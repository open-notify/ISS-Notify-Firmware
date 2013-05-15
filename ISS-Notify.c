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
#include "structs.h"
#include "eeproms.h"
#include "library/LUFA/Version.h"
#include "library/LUFA/Drivers/USB/USB.h"
#include "Descriptors.h"
#include "hardware/led.h"
#include "hardware/analog.h"
#include "hardware/rtc.h"
#include "hardware/charge.h"
#include "commands.h"
#include "ISS-Notify.h"

// Hardware initializer routine
void Setup_Hardware(void);

// File-like object for talking over USB
FILE USBSerialStream;

/** 
 * LUFA CDC Class driver interface configuration and state information. This structure is
 * passed to all CDC Class driver functions, so that multiple instances of the same class
 * within a device can be differentiated from one another.
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

/* LUFA USB Events */
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

/* variables that get updated by interupts */
volatile uint8_t reset_alarm = 0;
volatile uint8_t alarm       = 0;
volatile uint8_t pass        = 0;
volatile uint32_t ms         = 0;

/* Global Variables */
bool do_rainbow  = false;
uint32_t previousMillis = 0;
uint8_t bow_rotate = 0;

/* Data stored in eeprom (stable across power resets */
uint16_t EEMEM NonVolatileColor;
uint8_t EEMEM NonVolatile_Blue_WB;
uint8_t EEMEM NonVolatile_Green_WB;
uint8_t EEMEM NonVolatile_Red_WB;
uint8_t EEMEM NumOfStoredISSPasses;
ipass EEMEM StoredISSPasses[MAXPASS];

/* Current color output to the LEDs */
uint16_t show[8][3];

// A rainbow!
uint16_t rainbow[8][3] = {{0xffff,0x0000,0x0000}, {0xffff,0xbfff,0x0000}, {0x7fff,0xffff,0x0000}, {0x0000,0xffff,0x3fff}, {0x0000,0xffff,0xffff}, {0x0000,0x3fff,0xffff}, {0x7fff,0x0000,0xffff}, {0xffff,0x0000,0xbfff}};

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
	uint8_t i, j;

	// Setup hardware
	Setup_Hardware();

	
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

	// Startup animation
	for (i=0;i<100;i++)
	{
		for (j=0;j<8;j++)
		{
			show[j][0] = i*655;
			show[j][1] = i*655;
			show[j][2] = i*655;
		}
		led(show);
		_delay_ms(1);
	}
	for (i=100;i>1;i--)
	{
		for (j=0;j<8;j++)
		{
			show[j][0] = i*655;
			show[j][1] = i*655;
			show[j][2] = i*655;
		}
		led(show);
		_delay_ms(2);
	}
	for (j=0;j<8;j++)
	{
		show[j][0] = 0;
		show[j][1] = 0;
		show[j][2] = 0;
	}
	led(show);

	/***************************************************************************
	* THE MAIN LOOP
	***************************************************************************/
	while(1) {

		// Listen for commands over USB serial
		char c;
		if (fscanf(&USBSerialStream,"ISS%c\n", &c) == 1) {
			for (i=0;i<N_CMDS;i++) {
				cmd command = COMMANDS[i];
				if (command.c == c) {
					command.resp();
					break;
				}
			}
		}

		// When an alarm is triggered by the RTC:
		if (alarm > 0)
		{
			// Turn off alarm
			clear_alarm0();

			pass = 1;

			// Renenable alarm interupt
			EIMSK |=  (1<<INT3);
			alarm = 0;
		}

		if (do_rainbow) {
			uint32_t currentMillis = millis();
			if(currentMillis - previousMillis > 80) {
				previousMillis = currentMillis;   
				bow_rotate += 1;
			}
			for (j=0;j<8;j++)
			{
				for (i=0;i<3;i++)
					if ((j + bow_rotate) > 7)
						show[j][i] = rainbow[bow_rotate-(8-j)][i];
					else
						show[j][i] = rainbow[j+bow_rotate][i];
			}
			if (bow_rotate > 7)
				bow_rotate = 0;
			led(show);
		}
		led(show);

		// USB Service
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();

		if (reset_alarm > 0)
		  clear_alarm0();
		  reset_alarm = 0;
		  // Renenable interupts
		  EIMSK |=  (1<<INT3);

  	} // END MAIN LOOP

} // END MAIN

/**
 * Helper funtion to get current number of milliseconds since boot
 * ms is volitile and updated in a timer
 */
uint32_t millis(void) { return ms; }

/* Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	return;
}

/* Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	return;
}

/* Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/* Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
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
	TLC_Blue_WB = eeprom_read_byte((uint8_t*) &NonVolatile_Blue_WB);
	TLC_Green_WB = eeprom_read_byte((uint8_t*) &NonVolatile_Green_WB);
	TLC_Red_WB = eeprom_read_byte((uint8_t*) &NonVolatile_Red_WB);

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
