//create_lib.h
//make a control c safe
//measure uncertainties

#include <stdio.h>
#include <iostream>
#include <math.h>
#include "iwspy.h"

/**
 -----
 * Contains Create PlayerStage API.
 -----
 */

//function prototypes
int init_player_client_dcop();
int player_create_init();
float move_forward(float distance);
int turn(int degrees);
int stop();
