#ifndef CONFIG_H
#define CONFIG_H

//DCOP Constants
#define NUM_AGENTS 2
#define NUM_ROUNDS 10
#define DCOP_ALGORITHM "MPGM1"
#define AVG_SIGNAL_STR 27.0
#define MAX_SIGNAL_STR 100.0
#define I_PARAM 38.0
#define MOVEMENT_WAVE_LENGTH 0.06
#define MOVEMENT_DRIVER_LOC "null"

//load configuration
void loadConfiguration();

#endif
