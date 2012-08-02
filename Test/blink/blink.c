/**
 * Simple Blink a Ligh through the breakout header
 * ISS-Notify
 */
#include <avr/io.h>
#include <util/delay.h>

// LED is on port D, pin 6 which is "6" on the breakout header
#define LED_ON		(PORTD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))
#define LED_CONFIG	(DDRD |= (1<<6))

// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

int main(void)
{
	// set for 8 MHz clock (see Makefile), and make sure the LED is off
	CPU_PRESCALE(0);
	LED_CONFIG;
	LED_OFF;

	// Loop and blink the light
	while (1) {
	  LED_ON;
	  _delay_ms(100);
	  LED_OFF;
	  _delay_ms(100);
	}
}
