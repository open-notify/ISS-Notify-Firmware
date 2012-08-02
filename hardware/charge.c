#include <avr/io.h>
#include <stdint.h>
#include "hardware/analog.h"
#include "charge.h"

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
