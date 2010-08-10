//create_lib.h
//make a control c safe
//measure uncertainties

#include <stdio.h>
#include <iostream>
#include <libplayerc++/playerc++.h>
#include <math.h>
#include "iwspy.h"

using namespace PlayerCc;

/**
 -----
 * Contains Create PlayerStage API.
 -----
 */

//Player Create parameters
extern int port;
extern char* host;
extern PlayerClient* robot;
extern Position2dProxy* pp;
extern BumperProxy* bp;

//function prototypes
int init_player_client_dcop();
int player_create_init();
float move_forward(float distance);
int norm_degrees (int degrees);
float norm_radians (float radians);
int turn(int degrees);
int stop();