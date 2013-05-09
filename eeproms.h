// Maximum number of passes EEPROM can hold
#define MAXPASS 10

// EEPROM Storage
uint16_t EEMEM NonVolatileColor;
uint8_t EEMEM NonVolatile_Blue_WB;
uint8_t EEMEM NonVolatile_Green_WB;
uint8_t EEMEM NonVolatile_Red_WB;
//uint8_t EEMEM NonVolatileOptions;
uint8_t EEMEM NumOfStoredISSPasses;
ipass EEMEM StoredISSPasses[MAXPASS];
