#define MAX_BUFFER_LENGTH 8

#define RTC_DEVICE_ADDRESS 0x6f

#define RTC_ST      0x80
#define RTC_VBATEN  0x08

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} time;

void RTC_Init(void);
time get_time(void);
void set_time(time t);
void reset_rtc(void);

uint8_t read_address(uint8_t address);
uint8_t write_address(uint8_t address, uint8_t data);
uint8_t dec2bcd(uint8_t val);
uint8_t bcd2dec(uint8_t val);
time mktime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
