#ifndef MODEM_H
#define MODEM_H

#include <stdbool.h>

#define TRACKER_ENDPOINT  "http://YOUR_APPS_SCRIPT_URL_HERE"

/*
 * APN for your SIM.  Beeline KZ example: "internet.beeline.kz"
 */
#define MODEM_APN  "internet"

void modem_init(void);

bool modem_send_location(double lat, double lon);

void modem_sleep(void);

void modem_wake(void);

#endif
