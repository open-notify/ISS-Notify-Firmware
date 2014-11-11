///TODO: Fix values
#define MAX_BAT_ADC 390
#define MIN_BAT_ADC 280

// The battery voltage can be read off of pin PB6
// The mux value for the ATMEGA32U4 is 0x25
#define BAT_SENSE 0x25

#define STAT1 2
#define STAT2 7
#define PG 6

#define STAT1_READ (PIND & (1<<STAT1))
#define STAT2_READ (PINB & (1<<STAT2))
#define PG_READ    (PINC & (1<<PG))

// Charge Status Enum
typedef enum CHARGE_STATUS {
  NONE,
  BULK_CHARGE,
  TRICKLE_CHARGE
} CHARGE_STATUS;

void charge_Init(void);

// Measure battery voltage
uint8_t get_battery_voltage(void);
CHARGE_STATUS get_charge_status(void);
uint8_t get_power_status(void);
