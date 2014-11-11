// Maximum number of passes EEPROM can hold
#define MAXPASS 10

// EEPROM Storage
extern uint16_t EEMEM NonVolatileColor;
extern uint8_t EEMEM NonVolatile_Blue_WB;
extern uint8_t EEMEM NonVolatile_Green_WB;
extern uint8_t EEMEM NonVolatile_Red_WB;
//uint8_t EEMEM NonVolatileOptions;
extern uint8_t EEMEM NumOfStoredISSPasses;
extern ipass EEMEM StoredISSPasses[MAXPASS];
