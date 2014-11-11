#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "structs.h"
#include "eeproms.h"
#include "../lib/vendor/lufa/LUFA/Version.h"
#include "../lib/vendor/lufa/LUFA/Drivers/USB/USB.h"
#include "hardware/rtc.h"
#include "hardware/led.h"
#include "ISS-Notify.h"
#include "commands.h"


/*******************************************************************************
* COMMANDS
*******************************************************************************/


/**
 * Acknowledge. Lets a client know that it's here and talking.
 * Replies: 'ack' to the USB serial stream
 */
void ack(void)
{
	fputs("ack", &USBSerialStream);
}


/**
 * Say milliseconds
 * Replies: current number of milliseconds since boot. For debug purposes.
 */
void say_ms(void)
{
	fprintf(&USBSerialStream, "%lu", millis());
}


/**
 * Set the user defined color for diplaying passes
 * Replies: 'set' if successfull
 */
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


/**
 * Say Time. Will repsond with a date time stamp from the RTC
 * Replies: timestamp
 */
void say_time(void)
{
	time now = get_time();
	fprintf(&USBSerialStream, "Y%02dM%02dD%02dH%02dM%02dS%02d", now.year, now.month, now.day, now.hour, now.minute, now.second);
}


/**
 * Set upcoming passes. Writes the incoming pass data to EEPROM storage
 */
void set_passes(void)
{
	// Parse incoming data
	uint8_t i;
	int num;
	ipass block[MAXPASS];
	if (fscanf(&USBSerialStream,"%d\n", &num) == 1) {
		// Loop through incoming data
		for (i=0;i<num;i++) {
			uint32_t t;
			uint16_t d;
			ipass p;
			
			if (fscanf(&USBSerialStream, "%lu,%u,", &t, &d) == 2) {
				p.time = t;
				p.duration = d;
				block[i] = p;
			}
		}

		// Write to eeprom
		eeprom_update_block((const void * ) &block, (void*) &StoredISSPasses, sizeof(ipass)*num);
		eeprom_update_byte((uint8_t*) &NumOfStoredISSPasses, (uint8_t) num);

		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Dump Current Memory. Dumps the EEPROM blocks over USB stream
 * Mostly for debug.
 */
void dump_mem(void)
{
	uint8_t i;
	uint16_t color;
	color = eeprom_read_word((uint16_t*) &NonVolatileColor);

	uint8_t npass;
	npass = eeprom_read_byte((uint8_t*) &NumOfStoredISSPasses);
	if (npass > MAXPASS)
		npass = MAXPASS;

	// Echo data
	fprintf(&USBSerialStream, "0x00 - color: %d|", (int) color);
	fprintf(&USBSerialStream, "0x01 - npasses: %d|", (int) npass);


	ipass block[MAXPASS];
	eeprom_read_block((void*) &block, (const void *) &StoredISSPasses, sizeof(ipass)*npass);
	for (i=0;i<npass;i++) {
		fprintf(&USBSerialStream, "0x%02d - pass: %lu:%u|", i+2, block[i].time, block[i].duration);
	}
}


/**
 * Set The Date and Time on the RTC.
 */
void set_clock(void)
{
	reset_rtc();

	// set time
	int year, month, day, hour, min, sec;
	if (fscanf(&USBSerialStream,"Y%dM%dD%dH%dM%dS%d", &year, &month, &day, &hour, &min, &sec) == 6) {
		time t = mktime((uint8_t) year, (uint8_t) month, (uint8_t) day, (uint8_t) hour, (uint8_t) min, (uint8_t) sec);
		set_time(t);

		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Show a red value of for whitebalance
 */
void wb_red(void)
{
	uint8_t i;
	int v;
	if (fscanf(&USBSerialStream,"%d", &v) == 1) {
		for (i=0;i<8;i++) {
			show[i][0] = 0xffff;
			show[i][1] = 0xffff;
			show[i][2] = 0xffff;
		}
		TLC_Red_WB = v;
		led(show);
		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Show a green value of for whitebalance
 */
void wb_green(void)
{
	uint8_t i;
	int v;
	if (fscanf(&USBSerialStream,"%d", &v) == 1) {
		for (i=0;i<8;i++) {
			show[i][0] = 0xffff;
			show[i][1] = 0xffff;
			show[i][2] = 0xffff;
		}
		TLC_Green_WB = v;
		led(show);
		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Show a blue value of for whitebalance
 */
void wb_blue(void)
{
	uint8_t i;
	int v;
	if (fscanf(&USBSerialStream,"%d", &v) == 1) {
		for (i=0;i<8;i++) {
			show[i][0] = 0xffff;
			show[i][1] = 0xffff;
			show[i][2] = 0xffff;
		}
		TLC_Blue_WB = v;
		led(show);
		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Set the whitebalance.
 */
void set_wb(void)
{
	uint8_t i;
	int b,g,r;
	if (fscanf(&USBSerialStream,"%d,%d,%d", &b,&g,&r) == 3) {
		for (i=0;i<8;i++) {
			show[i][0] = 0;
			show[i][1] = 0;
			show[i][2] = 0;
		}
		TLC_Blue_WB = b;
		TLC_Green_WB = g;
		TLC_Red_WB = r;
		eeprom_update_byte((uint8_t*) &NonVolatile_Blue_WB, (uint8_t) b);
		eeprom_update_byte((uint8_t*) &NonVolatile_Green_WB, (uint8_t) g);
		eeprom_update_byte((uint8_t*) &NonVolatile_Red_WB, (uint8_t) r);

		led(show);
		fputs("set", &USBSerialStream);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Updates all the LEDs to a user defined color
 */
void show_color(void)
{
	int b,g,r,i;
	if (fscanf(&USBSerialStream,"%d,%d,%d", &b,&g,&r) == 3) {
		for (i=0;i<8;i++) {
			show[i][0] = b;
			show[i][1] = g;
			show[i][2] = r;
		}
		led(show);
	}
	else
		fputs("err", &USBSerialStream);
}


/**
 * Easteregg: Toggle Rainbow mode.
 */
void toggle_rainbow(void)
{
	int i;
	if (do_rainbow) {
		do_rainbow = false;
		for (i=0;i<8;i++) {
			show[i][0] = 0;
			show[i][1] = 0;
			show[i][2] = 0;
		}
		led(show);
	}
	else
		do_rainbow = true;
}


/** TODO:

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
*/
