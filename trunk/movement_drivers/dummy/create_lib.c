//create_lib.h
//make a control c safe
//measure uncertainties

#include <stdio.h>
#include <iostream>
#include <math.h>
#include "create_lib.h"

using namespace std;


/**
 -----
 * Contains Create PlayerStage API.
 -----
 */
int init_player_client_dcop() {
	return 1;
}

int player_create_init() {
	cout<<"Robot created."<<endl;
	return 1;
}

float move_forward(float distance) {
	cout<<"Robot moved "<<distance<<"."<<endl;
	sleep(2);
	return 1.0;
}

int turn(int degrees) {
	cout<<"Robot turned "<<degrees<<" degrees."<<endl;
	return 1;
}

int stop() {
	cout<<"Robot stopped"<<endl;
	return 1;
}
