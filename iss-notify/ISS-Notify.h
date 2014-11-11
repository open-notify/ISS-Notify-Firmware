// CPU prescaler helper
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

/**
 * Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 * used like any regular character stream in the C APIs
 */
extern FILE USBSerialStream;

// Helper for counting milliseconds
uint32_t millis(void);

// Global variable that always stores current color
extern uint16_t show[8][3];

extern bool do_rainbow;
extern uint32_t previousMillis;
extern uint8_t bow_rotate;

/* LUFA USB Events */
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

