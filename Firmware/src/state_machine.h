#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define CYCLE_INTERVAL_S  300  

#define GPS_FIX_TIMEOUT_MS  90000 

void state_machine_init(void);

void state_machine_run(void);

#endif
