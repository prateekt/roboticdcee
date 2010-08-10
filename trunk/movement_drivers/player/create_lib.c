//create_lib.h
//make a control c safe
//measure uncertainties

#include <stdio.h>
#include <iostream>
#include <libplayerc++/playerc++.h>
#include <math.h>
#include "create_lib.h"
#include <sstream>

using namespace std;


/**
 -----
 * Contains Create PlayerStage API.
 -----
 */

//create parameters
int port = 6665;
char* host = "localhost";
PlayerClient* robot;
Position2dProxy* pp;
BumperProxy* bp;

int init_player_client_dcop() {
	stringstream msg;
	msg << "player ";
	msg << PLAYER_LOC;
	exec(msg.str());
	player_create_init();
}

int player_create_init() {
	robot = new PlayerClient( host, port );
	pp = new Position2dProxy( robot );
	bp = new BumperProxy( robot, 0 );
}

float move_forward(float distance) {
	player_create_init();
	robot->Read();
	float x1, x2, y1, y2, d;
	x1 = pp->GetXPos(); y1 = pp->GetYPos();
	pp->SetSpeed(1,0);
	while(true) {
		robot->Read();
		x2 = pp->GetXPos(); y2 = pp->GetYPos();
		d = sqrt( pow( (x2-x1), 2 ) + pow( (y2-y1), 2 ) ); //distance formula
		if (bp->IsAnyBumped() == TRUE) //if any bumpers are bumped, stop and return the distance traveled 
		{
			//multiple slow down commands in case the CPU is nonresponsive the first time.
			for(int f=0; f < 10; f++)
				pp->SetSpeed(0,0);
			stop();
			return d;
		}
		else if (d >= distance) {
			
			//multiple slow down commands in case the CPU is nonresponsive the first time.
			for(int f=0; f < 10; f++)
				pp->SetSpeed(0,0);
			stop();
			return d;
		}
	}
}

/**
 * Normalizes degrees to a 0 to 360 range.
 */
int norm_degrees (int degrees) {
	if(degrees >= 360) {
		while(degrees >= 360) {
			degrees -= 360;
		}
		return degrees;
	}
	else if (degrees < 0) {
		while(degrees < 0) {
			degrees += 360;
		}
		return degrees;
	}
	else {
		return degrees;
	}
}

/**
 * Normalizes radians to a 0-->2*M_PI range.
 */
float norm_radians (float radians) {
	if(radians >= 2*M_PI) {
		while(radians >= 2*M_PI) {
			radians -= 2*M_PI;
		}
		return radians;
	}
	else if (radians < 0) {
		while(radians < 0) {
			radians += 2*M_PI;
		}
		return radians;
	}
	else {
		return radians;
	}
}


int turn(int degrees) {
	player_create_init();
	float pos1, pos2, posd;
	robot->Read();
	pos1 = norm_degrees(RTOD(pp->GetYaw()));
	posd = norm_degrees(pos1 + degrees);
	pos2 = norm_degrees(RTOD(pp->GetYaw()));
	cout<<"Start: "<<pos1<<endl;
	cout<<"Trying to get to: "<<posd<<endl;

	//regular cases
	if (pos2 <= posd) {
		cout<<"Case1"<<endl;
		pp->SetSpeed(0.0,0.5);
		while (pos2 < posd) {
			robot->Read();
			pos2 = norm_degrees(RTOD(pp->GetYaw()));
			cout<<"NEW POS2: "<<pos2<<endl;
		}
		for(int f=0; f < 10; f++) {
			pp->SetSpeed(0.0,0.0);
		}
		stop();
	}
	else if (pos2 > posd) {
		cout<<"Case2"<<endl;
		pp->SetSpeed(0,-0.5);
		while (pos2 > posd) {
			robot->Read();
			pos2 = norm_degrees(RTOD(pp->GetYaw()));
			cout<<"NEW POS2: "<<pos2<<endl;
		}
		for(int f=0; f< 10; f++){
			pp->SetSpeed(0.0,0.0);
		}
		stop();
	}

	//ultimate brake
	for(int f=0; f < 200; f++) {
		pp->SetSpeed(0.0,0.0);
	}

	cout<<"DONE TURNING"<<endl;
	return 0;
}

int stop() {
	exec("player ~/player.cfg");
	return 1;
}
