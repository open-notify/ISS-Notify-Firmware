#include <avr/io.h>
#include <stdint.h>
#include "../../lib/twi.h"
#include "rtc.h"

static uint8_t rtc_state;
static uint8_t tx_buffer[MAX_BUFFER_LENGTH];
static uint8_t rx_buffer[MAX_BUFFER_LENGTH];

void RTC_Init(void)
{
	twi_init();

	//interal pullup for alarm interrupt
	DDRD  &= ~(1<<3);
	PORTD |=  (1<<3);
	
	// interrupt on low
	EICRA &= ~(1<<ISC30)|~(1<<ISC31); //Active when low 
	EIMSK |=  (1<<INT3);              //external pin interrupt enable.
}

time get_time(void)
{
  uint8_t seconds = bcd2dec(read_address(0x00) & 0x7f);
  uint8_t minutes = bcd2dec(read_address(0x01) & 0x7f);
  uint8_t hour    = bcd2dec(read_address(0x02) & 0x3f);
  
  uint8_t day     = bcd2dec(read_address(0x04) & 0x3f);
  uint8_t month   = bcd2dec(read_address(0x05) & 0x1f);
  uint8_t year    = bcd2dec(read_address(0x06)       );
  
  time currentTime = mktime(year, month, day, hour, minutes, seconds);
  
  return currentTime;
}

void set_time(time t)
{
  uint8_t seconds = dec2bcd(t.second & 0x7f);
  uint8_t minutes = dec2bcd(t.minute & 0x7f);
  uint8_t hour    = dec2bcd(t.hour   & 0x3f);
  
  uint8_t day     = dec2bcd(t.day    & 0x3f);
  uint8_t month   = dec2bcd(t.month  & 0x1f);
  uint8_t year    = dec2bcd(t.year         );
  
  tx_buffer[0] = 0x00;
  tx_buffer[1] = seconds + 0x80;
  tx_buffer[2] = minutes;
  tx_buffer[3] = hour;
  
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 4, 0);
  
  tx_buffer[0] = 0x04;
  tx_buffer[1] = day;
  tx_buffer[2] = month;
  tx_buffer[3] = year;
  
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 4, 0);
}

void clear_alarm0(void)
{
  tx_buffer[0] = 0x0D;      // Alarm0 controll
  tx_buffer[1] = 0;
  
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 2, 0);
  
  
  // turn off alarm
  tx_buffer[0] = 0x07;
  tx_buffer[1] = (1<<RTC_OUT);
  
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 2, 0);
}

void set_alarm0(time t)
{
  uint8_t seconds = dec2bcd(t.second & 0x7f);
  uint8_t minutes = dec2bcd(t.minute & 0x7f);
  uint8_t hour    = dec2bcd(t.hour   & 0x3f);
  
  uint8_t day     = dec2bcd(t.day    & 0x3f);
  uint8_t month   = dec2bcd(t.month  & 0x1f);
  uint8_t year    = dec2bcd(t.year         );
  
  int dow = read_address(0x03) & 0x07;
  
  tx_buffer[0] = 0x0A;      // Beginning of Alarm0
  tx_buffer[1] = seconds;
  tx_buffer[2] = minutes;
  tx_buffer[3] = hour;
  tx_buffer[4] = (1<<ALM0C2)|(1<<ALM0C1)|(1<<ALM0C0)|dow;
  tx_buffer[5] = day;
  tx_buffer[6] = month;
  tx_buffer[7] = year;
    
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 8, 0);

  // turn on alarm
  tx_buffer[0] = 0x07;
  tx_buffer[1] = (1<<RTC_ALM0);
  
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 2, 0);
}

void reset_rtc(void)
{
  // Enable battary backup
  write_address(0x03, RTC_VBATEN);
  
  // Start keeping time
  write_address(0x00, RTC_ST);
}

uint8_t read_address(uint8_t address)
{
  // Set send buffer to address
  tx_buffer[0] = address;
  
  // Send the address to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 1, 1);
  
  // Error check
  if (rtc_state != 0)
    return 0;
  
  // Listen for message from RTC
  uint8_t message_length = twi_readFrom(RTC_DEVICE_ADDRESS, rx_buffer, 1);
  
  // Recieved nothing
  if (message_length == 0)
    return 0;

  // Return the byte from the address
  return rx_buffer[0];
}

uint8_t write_address(uint8_t address, uint8_t data)
{
  // Set send buffer to address and payload
  tx_buffer[0] = address;
  tx_buffer[1] = data;
  
  // Send to the RTC
  rtc_state = twi_writeTo(RTC_DEVICE_ADDRESS, tx_buffer, 2, 0);
  
  //twi_stop();
  //twi_releaseBus();
  //TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

  
  return rtc_state;
}

// Create time type
time mktime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  time t;
  
  t.year    = year;
  t.month   = month;
  t.day     = day;
  t.hour    = hour;
  t.minute  = minute;
  t.second  = second;
  
  return t;
}

// Convert normal decimal numbers to binary coded decimal
uint8_t dec2bcd(uint8_t val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
uint8_t bcd2dec(uint8_t val)
{
  return ( (val/16*10) + (val%16) );
}
