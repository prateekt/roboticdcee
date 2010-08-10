#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include "Network.h"
#include "dcop.h"
#include "iwspy.h"
#include "create_lib.h"
#include "sampler.h"
#include <vector>
#include "config.h"

using namespace std;

/**
 -----
 * Main file - contains main function that runs experiment sequence.
 -----
 */

int main(int argc, char* argv[]) {

	//Parse robot id parameter from command line
	if(argc < 2)  {
		cout<<"Need robot ID as parameter."<<endl;
		exit(0);
	}
	else {
		myRobotID = atoi(argv[1]);
	}
	
	//load configuration
	loadConfiguration();
	
	//initialize my global vars
	myLoc = 0;
	srand(time(NULL));
	visServerOn = VIS_SERVER_OFF;
	initDCOPSynch();

	//init player client
	init_player_client_dcop();

	//Connection to VisServer
/*	if(visServerOn) {
		visServerConn = connectToNode(VIS_SERVER_IP, VIS_SERVER_PORT+myRobotID);
		if(visServerConn==NULL)  {
			cout<<"Error: Could not connect to visualization server."<<endl;
			exit(0);
		}
	}*/

	//start servers for other robots
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		startAServer(myRobotID+ROBOT_NODE_PORT+neighbor*100,neighbor);
	}
	
	//Connect to other robots
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		initiateConnectionToNode(getIP(neighbor), neighbor+ROBOT_NODE_PORT+myRobotID*100, myRobotID+ROBOT_NODE_PORT+neighbor*100,neighbor);
	}
    cout<<"DONE CONNECT"<<endl;

	//Register with IWSPY
	bool regWithIWSPY = regAllWithIWSPY();

	//do sampling routine, if algorithm calls for it.
	cout<<"going to sampling"<<endl;
	executeSampling(DCOP_ALGORITHM);

	//tell other robots we can start the experiment
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		sendReadyToStartMsg(connections[neighbor]);
	}

	//TELL MYSELF I CAN START IF ALL ARE READY
	pthread_mutex_lock(&DCOPThreadLock_start);
	numReadyToGo++;
	cout<<"NumReadyToGo: "<<numReadyToGo<<" out of "<<(neighbors->size()+1)<<endl;
	if(numReadyToGo==neighbors->size()+1) {
		cout<<"Ready to go signaled."<<endl;
		pthread_cond_signal(&DCOPCV_start);
	}
	pthread_mutex_unlock(&DCOPThreadLock_start);
	cout<<"done main"<<endl;

	//main thread needs to stay alive for duration
	//of experiment. Have it loop until DCOP code is done.
	while(numRoundsElapsed < NUM_ROUNDS) {
		sched_yield();
	}

	//close connections
	closeConnections();

	//wait till all ports are closed before exiting
	pthread_mutex_lock(&endLock);
	if(numPortsClosed!=numPorts) {
		cout<<"Waiting for all ports to close: "<<numPortsClosed<<"/"<<numPorts<<endl;
		pthread_cond_wait(&endCV, &endLock);
	}
	pthread_mutex_unlock(&endLock);
}

/* --SAMPLING ROUTINE WITH MARCOS'S SCRIPT */

/*int main(int argc, char* argv[]) {
	vector<string>* results = exec("sh ~/scripts/rssiSample.sh");
	for(vector<string>::const_iterator it = results->begin(); it!=results->end(); ++it) {
		string s = *it;
		cout<<"Line: "<<s;
		int i = atoi(s.c_str());
		cout<<"The int "<<i<<endl;

	}
}*/


/* --SAMPLING ROUTINE TEST CASE WITH MOTION-- */

/*int main(int argc, char* argv[]) {

	myRobotID = atoi(argv[1]);

	//Register with IWSPY
	bool regWithIWSPY = regAllWithIWSPY();
	if(!regWithIWSPY) {
		cout<<"IWSPY failed for some agent."<<endl;
		exit(0);
	}
	player_create_init();
	cout<<"GETTING SAMPLES"<<endl;
	getSamples(100, 5, 1);

	cout<<"Mean: "<<mean<<endl;
	cout<<"StdDev: "<<stddev<<endl;
	cout<<"Num Samples: "<<numSignalStrSamples<<endl;
}
*/


/*--IWSPY TEST CASE--*/

/*
int main(int argc, char* argv[]) {
	regAllWithIWSPY();

	map<int, int>* vals = getIWSPYResultsByAgent();
	int rtn[5];
	for(int x=0; x < 5; x++) {
		rtn[x] = -1;
	}
	for(map<int, int>::iterator i1 = vals->begin(); i1!=vals->end(); ++i1)	{
		rtn[(*i1).first] = (*i1).second;

	}

	for(int x=0; x < 5; x++) {
		cout<<rtn[x]<<" ";
	}
}*/


/*--MOVE TEST CASE--*/

/*
int main(int argc, char* argv[]) {
	myLoc = 2;
	rounds[0].ifIWinMyLoc = 0;
	move_BeRebid1();
}*/

