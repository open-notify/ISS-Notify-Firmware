///TODO: Fix values
#define MAX_BAT_ADC 390
#define MIN_BAT_ADC 280

// The battery voltage can be read off of pin PB6
// The mux value for the ATMEGA32U4 is 0x25
#define BAT_SENSE 0x25

// Measure battery voltage
uint8_t get_battery_voltage(void);
