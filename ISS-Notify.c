/**
 * ISS-Notify
 */
#include <avr/io.h>
#include <util/delay.h>

// LED is on port D, pin 6 which is "6" on the breakout header
#define LED_ON     (PORTD |=  (1<<6))
#define LED_OFF    (PORTD &= ~(1<<6))
#define LED_CONFIG (DDRD  |=  (1<<6))

// LED Drivers:
#define BLANK_HIGH  (PORTF |=  (1<<5))
#define BLANK_LOW   (PORTF &= ~(1<<5))

#define LATCH_HIGH  (PORTF |=  (1<<4))
#define LATCH_LOW   (PORTF &= ~(1<<4))

#define SIN_HIGH    (PORTF |=  (1<<1))
#define SIN_LOW     (PORTF &= ~(1<<1))

#define SCLK_HIGH   (PORTF |=  (1))
#define SCLK_LOW    (PORTF &= ~(1))

// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

void shift_out(int byte);

int main(void)
{
  int i;
  
	// set for 8 MHz clock (see Makefile), and make sure the LED is off
	CPU_PRESCALE(0);
  LED_CONFIG;
	LED_OFF;
	
	
	// LED Drivers:
	DDRF |= 51; //(000110011)
	SIN_LOW;
	SCLK_LOW;
	LATCH_LOW;
	BLANK_HIGH;
	
	// JTAG Disable
  MCUCR = (1 << JTD); 
	MCUCR = (1 << JTD); 
	
	/**
	 * Color Map
	shift_out(7392); //blue?
	_delay_ms(10);
	shift_out(28896); //Green
	_delay_ms(10);
	shift_out(50912);  // Red
	*/
	
	// Loop
	while (1) {
	  for (i=4;i<16;i++) {
	    BLANK_HIGH;
	    shift_out(1<<i);
	    BLANK_LOW;
	    _delay_ms(50);
	  }
	}
}

void shift_out(int byte)
{
  int i = 0;
  LATCH_HIGH;
  LATCH_LOW;
  
  //SIN_HIGH;
  for (i=0;i<16;i++)
  {
    // Bit
    if ((byte & (1 << i)) != 0)
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
