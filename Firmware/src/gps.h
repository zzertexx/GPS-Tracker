#ifndef GPS_H
#define GPS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double  latitude;
    double  longitude;
    uint8_t satellites;
    bool    valid;
} gps_fix_t;

void gps_init(void);

bool gps_acquire_fix(gps_fix_t *fix, int timeout_ms);

void gps_enter_standby(void);

bool gps_feed_byte(uint8_t byte);

gps_fix_t gps_get_fix(void);

#endif
