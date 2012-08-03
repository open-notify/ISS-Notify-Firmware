#include <avr/io.h>
#include <stdint.h>
#include "hardware/analog.h"
#include "charge.h"

/**
 * Hardware init
 */
void charge_Init(void)
{
  // Pull ups on battary stat
  DDRD &= ~(1<<STAT1);
  PORTD |= (1<<STAT1);
  
  DDRB &= ~(1<<STAT2);
  PORTB |= (1<<STAT2);
  
  DDRC &= ~(1<<PG);
  PORTC |= (1<<PG);
  
  // Bat measure
	DDRB |= 0x01;
}

uint8_t get_battery_voltage(void)
{
    // Enable battery measurment
    PORTB |=  1;
    
    // Read voltage on battery sense pin
	  uint16_t val = adc_read(BAT_SENSE);
	  
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

CHARGE_STATUS get_charge_status(void)
{
  if (STAT1_READ > 0)
    return BULK_CHARGE;
    
  if (STAT2_READ > 0)
    return TRICKLE_CHARGE;

  return NONE;
}

uint8_t get_power_status(void)
{
  return PG_READ;
}
