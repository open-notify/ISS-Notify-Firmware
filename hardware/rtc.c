#include <avr/io.h>
#include <stdint.h>
#include "library/twi.h"
#include "rtc.h"

static uint8_t rtc_state;
static uint8_t tx_buffer[MAX_BUFFER_LENGTH];
static uint8_t rx_buffer[MAX_BUFFER_LENGTH];

void RTC_Init(void)
{
  twi_init();
}

uint32_t get_time(void)
{
  uint8_t seconds = bcd2dec(read_address(0x00) & 0x7f);
  uint8_t minutes = bcd2dec(read_address(0x01) & 0x7f);
  uint8_t hour    = bcd2dec(read_address(0x02) & 0x3f);
  
  uint8_t day     = bcd2dec(read_address(0x04) & 0x3f);
  uint8_t month   = bcd2dec(read_address(0x05) & 0x1f);
  uint8_t year    = bcd2dec(read_address(0x06)       );
  
  time currentTime = mktime(year, month, day, hour, minutes, seconds);
  
  return time2unix(currentTime);
  //return (uint32_t) seconds;
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

// Convert year/month/day to unix time
uint32_t time2unix(time t)
{
  // Temp values
  int32_t  a, y, m;
  uint16_t lyear;
  
  int32_t  JDN;
  double    JD;
  double    timediff;
  
  // returned unix time
  uint32_t unixtime;

  // Fix 8 bit year
  if (t.year < 70)
    lyear = t.year + 2000;
  else
    lyear = t.year + 1900;


  // Julian Day number
  a   = (14 - t.month) / 12;
  y   = lyear + 4800 - a;
  m   = t.month + (12*a) - 3;
  JDN = (int32_t) t.day + (((153*m) + 2) / 5) + (365*y) + (y/4) - (y/100) + (y/400) - 32045;
  
  // Diff to unix epoch
  timediff = (double) JDN - 2440587.5;
  
  // to seconds
  unixtime = (uint32_t) timediff*86400.0;

  // Add back in partial days
  unixtime = unixtime + ((t.hour-12) * 3600) + (t.minute * 60) + t.second;

  return unixtime;
}
