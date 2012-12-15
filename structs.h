/**
 * Defines on ISS overhead pass
 */
typedef struct {
    uint32_t time;      // Time that the ISS rises [unix time]
    uint16_t duration;  // Set time - rise time [seconds]
} ipass;
