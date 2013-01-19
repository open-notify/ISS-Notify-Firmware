/**
 * Defines on ISS overhead pass
 */
typedef struct {
	uint32_t time;      // Time that the ISS rises [unix time]
	uint16_t duration;  // Set time - rise time [seconds]
} ipass;


/**
 * Commands that ISS Notify understands
 * contains a test character and a function to call if matched
 */
typedef struct {
	char c;             // Test chararacter
	void (*resp)(void); // Response function
} cmd;
