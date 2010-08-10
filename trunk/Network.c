#include <sys/socket.h>       //  socket definitions
#include <sys/types.h>        //  socket types
#include <arpa/inet.h>        //  inet (3) funtions
#include <unistd.h>           //  misc. UNIX functions
#include "Network.h"
#include "dcop.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <map>
#include "create_lib.h"
using namespace std;

/**
 -----
 * This file contains a lot of code!
 -----
 */

//configuration maps
map<int, string>* robotIDToIP;
map<string, int>* hwAddrToID;
map<int,vector<int>*>* neighborMap;

//convenience functions for map access
string getIP(int id) {
	return robotIDToIP->find(id)->second;
}

int getID(string hw) {
	return hwAddrToID->find(hw)->second;
}

vector<int>* getNeighbors(int id) {
	return neighborMap->find(id)->second;
}

/**
 * Returns whether a given robot is a neighbor of this robot.
 * @param rid The robot id
 * @return Whether robot of rid is a neighbor of current robot.
 */
bool isNeighbor(int rid) {
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		if(neighbor==rid)
			return true;
	}

	return false;
}

//Global variables
Connection* connections[NUM_AGENTS]; //array of connections to other agents
Connection* myServers[NUM_AGENTS]; //array of my server connections for other agents to connect to
Connection* visServerConn; //Connection to visualization server (optional)
bool visServerOn = false; //Whether VisServer is online - otherwise, ignore vis server code.
pthread_t* myServerThread; //Thread for my servers

//Network Message Management global variables
pthread_t* DCOPThread; //Thread DCOP runs in

//synchronization variables
int numReadyToGo; //variable to track number of agents that have given start flag
pthread_cond_t DCOPCV_start = PTHREAD_COND_INITIALIZER; //start condition variable
pthread_mutex_t DCOPThreadLock_start = PTHREAD_MUTEX_INITIALIZER; //start mutex
bool DCOPStarted =  false; //whether thread started or not

//round sync vars
Round rounds[NUM_ROUNDS]; //Round structs that hold sync variables during a round

//end sync
int numPortsClosed; //Number of ports closed at end of run
int numPorts; //number of ports in the system
pthread_cond_t endCV = PTHREAD_COND_INITIALIZER; //end condition variable
pthread_mutex_t endLock = PTHREAD_MUTEX_INITIALIZER; //end mutex


/**
 * Closes connections to all agents safely.
 */
void closeConnections() {

	//close connections to agents
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int i=0; i < neighbors->size(); i++) {
		int x = neighbors->at(i);
		if(connections[x]!=NULL) {
			close(connections[x]->conn_s);
			pthread_mutex_lock(&endLock);
			numPortsClosed++;
			pthread_mutex_unlock(&endLock);
		}
	}

	//if vis server on, close it
	if(visServerOn) {
		close(visServerConn->conn_s);
		pthread_mutex_lock(&endLock);
		numPortsClosed++;
		pthread_mutex_unlock(&endLock);
	}

	//update end state, if ready
	pthread_mutex_lock(&endLock);
	if(numPortsClosed==numPorts) {
		cout<<"SIGNALED_END"<<endl;
		pthread_cond_signal(&endCV);
	}
	pthread_mutex_unlock(&endLock);
}


/**
 * Inits synch primitives in round struct.
 */
void initDCOPSynch() {
	numReadyToGo = 0;

	for(int x=0; x < NUM_ROUNDS; x++) {

		//init sync primitives
		pthread_mutex_init(&rounds[x].preLock, NULL);
		pthread_cond_init(&rounds[x].preCV, NULL);
		rounds[x].preSignalVal = 0;
		pthread_mutex_init(&rounds[x].bidsLock, NULL);
		pthread_cond_init(&rounds[x].bidsCV, NULL);
		rounds[x].bidsSignalVal = 0;
		pthread_mutex_init(&rounds[x].movedLock, NULL);
		pthread_cond_init(&rounds[x].movedCV,NULL);
		rounds[x].movedSignalVal = 0;
		pthread_mutex_init(&rounds[x].neighborMatricesLock, NULL);
		pthread_cond_init(&rounds[x].neighborMatricesCV, NULL);
		rounds[x].neighborMatricesSignal = 0;
		pthread_mutex_init(&rounds[x].firstBidsLock, NULL);
		pthread_cond_init(&rounds[x].firstBidsCV, NULL);
		rounds[x].firstBidsSignal = 0;
		pthread_mutex_init(&rounds[x].secondBidsLock, NULL);
		pthread_cond_init(&rounds[x].secondBidsCV, NULL);
		rounds[x].secondBidsSignal = 0;
		pthread_mutex_init(&rounds[x].thirdBidsLock, NULL);
		pthread_cond_init(&rounds[x].thirdBidsCV, NULL);
		rounds[x].thirdBidsSignal = 0;
		pthread_mutex_init(&rounds[x].expectedLock, NULL);
		pthread_cond_init(&rounds[x].expectedCV, NULL);
		rounds[x].expectedSignalVal = 0;
	}
}

/**
 * Gets the time stamp - for use in message queuing.
 */
int getTimeStamp() {
	return time(NULL);
}

/**
 * Sends the third bid in a k=2 algorithm
 *
 * @param conn The connection to send the bid message to
 * @param roundNumber The current round number
 * @param pair The bid struct
 * @return Whether the message send was successful or not
 */
int sendThirdBid(Connection* conn, int roundNumber, K2Bid* pair) {
	stringstream msg;
	msg<<"T";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<pair->myPos;
	msg<<":";
	msg<<pair->neighborPos;
	msg<<":";
	msg<<pair->reward;
	msg<<":";
	msg<<pair->neighborID;
	return sendMessage(conn, msg.str());
}

/**
 * Sends the second bid in a k=2 algorithm
 *
 * @param conn The connection to send the bid message to
 * @param roundNumber The current round number
 * @param pair The bid struct
 * @return Whether the message send was successful or not
 */
int sendSecondBid(Connection* conn, int roundNumber, K2Bid* pair) {
	stringstream msg;
	msg<<"S";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<pair->myPos;
	msg<<":";
	msg<<pair->neighborPos;
	msg<<":";
	msg<<pair->reward;
	msg<<":";
	msg<<pair->neighborID;
	return sendMessage(conn, msg.str());
}


/**
 * Sends the first bid in a k=2 algorithm
 *
 * @param conn The connection to send the bid message to
 * @param roundNumber The current round number
 * @param pair The bid struct
 * @return Whether the message send was successful or not
 */
int sendFirstBid(Connection* conn, int roundNumber, K2Bid* pair) {
	stringstream msg;
	msg<<"F";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<pair->myPos;
	msg<<":";
	msg<<pair->neighborPos;
	msg<<":";
	msg<<pair->reward;
	msg<<":";
	msg<<pair->neighborID;
	return sendMessage(conn, msg.str());
}

/**
 * Sends a done moving message.
 *
 * @param conn The connection to send the message to
 * @param roundNumber The current round number
 * @return Whether the message send was successful or not
 */
int sendDoneMovingMsg(Connection* conn, int roundNumber) {
	stringstream msg;
	msg<<"M";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	return sendMessage(conn, msg.str());
}

/**
 * Sends an expected to move message. This is so the robot(s) that are expected
 * to move know to tell all the robots expecting them to move that they have
 * finished moving later. This message is there for synchronization purposes.
 *
 * @param conn The connection to send the bid message to
 * @param roundNumber The current round number
 * @param expectedToMove The first robot expected to move
 * @param expectedToMove2 In a k=2 algorithm, the other robot expected to move
 * @return Whether the message send was successful or not
 */
int sendExpectedToMoveMsg(Connection* conn, int roundNumber, int expectedToMove, int expectedToMove2) {
	stringstream msg;
	msg<<"E";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<expectedToMove;
	msg<<":";
	msg<<expectedToMove2;
	return sendMessage(conn, msg.str());
}

/**
 * Sends a bid message for a given round (k=1)
 *
 * @param conn The connection to send the bid message to
 * @param roundNumber The current round number
 * @param bid The round bid
 * @return Whether the message send was successful or not
 */
int sendRoundBidMsg(Connection* conn, int roundNumber, int bid) {
	stringstream msg;
	msg<<"B";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<bid;
	return sendMessage(conn, msg.str());
}

/**
 * Sends a preround value message. In DCEE,
 * a preround variable is the robot's current location.
 *
 * @param conn The connection to send the message to
 * @param roundNumber The current round number
 * @param value The value of the preround variable.
 * @return Whether the message send was successful or not
 */
int sendPreroundValueMsg(Connection* conn, int roundNumber, int value) {
	stringstream msg;
	msg<<"P";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<value;
	return sendMessage(conn, msg.str());
}

/**
 * Sends a ready to start message.
 *
 * @param conn The connection to send the bid message to
 * @return Whether the message send was successful or not
 */
int sendReadyToStartMsg(Connection* conn) {
	stringstream msg;
	msg<<"R";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	return sendMessage(conn, msg.str());
}

/**
 * Sends a cumulative signal strength message.
 *
 * @param conn The connection to send the bid message to
 * @param roundNumber The current round number
 * @param value the Value of the cumulative signal strength.
 * @return Whether the message send was successful or not
 */
int sendCumSignalStrMsg(Connection* conn, int roundNumber, int value) {
	stringstream msg;
	msg<<"C";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<value;
	return sendMessage(conn, msg.str());
}

/**
 * Sends an instantaneous signal strength (on a particular link) message.
 *
 * @param conn The connection to send the bid message to
 * @param with The robot whom the link is shared with
 * @param roundNumber The current round number
 * @param value The value of the link
 * @return Whether the message send was successful or not
 */
int sendInstSignalStrMsg(Connection* conn, int with, int roundNumber, int value) {
	stringstream msg;
	msg<<"I";
	msg<<":";
	msg<<getTimeStamp();
	msg<<":";
	msg<<myRobotID;
	msg<<":";
	msg<<roundNumber;
	msg<<":";
	msg<<with;
	msg<<":";
	msg<<value;
	return sendMessage(conn, msg.str());
}

/**
 * Initiate connection with another robot.
 *
 * @param remoteAddr The remote address of the robot to connect to
 * @param remotePort The port of the robot to connect to
 * @param myServerPort The server port that I will lost for the other robot to connect to
 * @param otherRID The other robot's robot id parameter
 */
void initiateConnectionToNode(string remoteAddr, int remotePort, int myServerPort, int otherRID) {

	cout<<"starting a server for "<<otherRID<<" on "<<myServerPort<<endl;
	cout<<"awaiting conn for "<<otherRID<<" on "<<remotePort<<endl;

	//start a server for the other to respond to
//	startAServer(myServerPort, otherRID);

	//try connecting to the other
	Connection* c;
	for(int x=0; x < 1000000; x++) {

		//attempt connection to other
		c = connectToNode(remoteAddr, remotePort);
		if(c==NULL) {

			cout<<"Connection attempt for "<<remoteAddr<<"//"<<remotePort<<" failed...retrying (attempt "<<x<<")"<<endl;
			cout<<"The robot attempt was for "<<otherRID<<endl;

			//the waiting
			for(int y=0; y < 10000; y++) {
				sched_yield();
			}
		}
		else {
			break;
		}
	}

	//failure
	if(c==NULL) {
		cout<<"Totally failed to connect to node "<<remoteAddr<<"//"<<remotePort<<endl;
		exit(0);
	}
	else {
		//success
		c->robotID = otherRID;
		connections[otherRID] = c;

		//update port count for agent
		pthread_mutex_lock(&endLock);
		numPorts++;
		numPorts++;
		pthread_mutex_unlock(&endLock);
	}
}

/**
 * Sends a message exclusively to the vis server, if its on.
 *
 * @param message The message to send
 * @return Whether the message sent was successful or not.
 */
int sendToVisServer(string message) {

	//format message into network sendable format
	char wireSend[MAX_MESSAGE_SIZE];
	for(int x=0; x < MAX_MESSAGE_SIZE; x++) {
		if(x < message.size())
			wireSend[x] = message.at(x);
		else
			wireSend[x] =' ';
	}
	wireSend[MAX_MESSAGE_SIZE-1] = '\n';

	//send to vis server connection, if its on.
	if(visServerOn && visServerConn!=NULL) {
		Writeline(visServerConn->conn_s, wireSend, MAX_MESSAGE_SIZE);
		cout<<"Wrote Message "<<" on "<<visServerConn->conn_s<<": "<<wireSend;
	}

	return 1;
}

/**
 * Sends a message to any particular connection, if its on.
 * If the vis server is on, the message will also be forward to it.
 *
 * @param conn The connection
 * @param message The message to send
 * @return Whether the message sent was successful or not.
 */
int sendMessage(Connection* conn, string message) {

	//format message into network sendable format
	char wireSend[MAX_MESSAGE_SIZE];
	for(int x=0; x < MAX_MESSAGE_SIZE; x++) {
		if(x < message.size())
			wireSend[x] = message.at(x);
		else
			wireSend[x] =' ';
	}
	wireSend[MAX_MESSAGE_SIZE-1] = '\n';

	//send message over wire
	if(conn!=NULL) {
		Writeline(conn->conn_s, wireSend, MAX_MESSAGE_SIZE);
		cout<<"Wrote Message "<<" on "<<conn->conn_s<<": "<<wireSend;
	}

	//also forward to vis server connection, if its on.
	if(visServerOn && visServerConn!=NULL) {
		Writeline(visServerConn->conn_s, wireSend, MAX_MESSAGE_SIZE);
	}

	return 1;
}

/**
 * Connects to a particular node.
 *
 * @param remoteAddr The IP address of the node
 * @param port The port to connect to
 * @return Connection object if connection was successful
 */
Connection* connectToNode(string remoteAddr, int port) {
	int conn_s;
	struct    sockaddr_in servaddr;  //socket address structure
	Connection* rtn;

	//Create the listening socket
	if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "Error creating listening socket.\n");
		return NULL;
	}

	//  Set all bytes in socket address structure to
	// zero, and fill in the relevant data members
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_port        = htons(port);

	//  Set the remote IP address
	if (inet_aton(remoteAddr.c_str(), &servaddr.sin_addr) <= 0 ) {
		printf("Invalid remote IP address.\n");
		return NULL;
	}

	//connect() to the remote server
	if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
		printf("Error calling connect()\n");
		return NULL;
	}

	//Connection struct to store data globally
	rtn = new Connection();
	rtn->conn_s = conn_s;
	rtn->servaddr = servaddr;
	rtn->port = port;

	cout<<"Connected remotely to port "<<port<<" on "<<remoteAddr<<" conn_s: "<<conn_s<<endl;

	return rtn;
}

/**
 * Starts up a server on a given port for a particular neighboring robot.
 * This includes forking a new thread so that the server can run in its own
 * thread.
 *
 * @param port The port
 * @param otherID The other robot's ID
 * @return Whether the server for the other robot could be started.
 */
int startAServer(int port, int otherID) {
	myServers[otherID] = createAServerConnection(port);

	//set up a server to manage that server connection
	myServerThread = new pthread_t();
	if((pthread_create(myServerThread, NULL, &serverThreadFunc, (void*) otherID))!=0) {
		cout<<"Failure on setting up server thread. "<<endl;
		return  -1;
	}
	return 1;
}

/**
 * Trim function for a string - removes white spaces.
 *
 * @param s The string
 */
void trim(char *s)
{
	// Trim spaces and tabs from beginning:
	int i=0,j;
	while((s[i]==' ')||(s[i]=='\t')) {
		i++;
	}
	if(i>0) {
		for(j=0;j<strlen(s);j++) {
			s[j]=s[j+i];
		}
	s[j]='\0';
	}

	// Trim spaces and tabs from end:
	i=strlen(s)-1;
	while((s[i]==' ')||(s[i]=='\t')) {
		i--;
	}
	if(i<(strlen(s)-1)) {
		s[i+1]='\0';
	}
}

/**
 * The function that a new server thread should run. A server thread
 * will accept a connection from a connecting robot, accept receiving messages
 * from them, and continue receiving messages until the experiment is done.
 * When the experiment is done, the server will terminate itself and inform
 * the other robot that is has terminated.
 *
 * @param arg The ID of the other robot.
 */
void* serverThreadFunc(void* arg) {

	int otherID = (int) arg;
	Connection* conn = myServers[otherID];
	int list_s  = conn->list_s;
	struct sockaddr_in servaddr = conn->servaddr;
	char buffer[MAX_MESSAGE_SIZE];

	cout<<"Starting Server Thread for "<<otherID<<" for port "<<conn->port<<" using "<<list_s<<endl;
	int conn_s;

	//  Wait for a connection, then accept() it
	if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
		fprintf(stderr, "Error calling accept()\n");
		exit(EXIT_FAILURE);
	}

	while (numRoundsElapsed < NUM_ROUNDS) {
		recv(conn_s, buffer, MAX_MESSAGE_SIZE,0);
		if(buffer[1]!=':') {
			cout<<"ERRONEOUS, DISCARDED MESSAGE: "<<buffer<<endl;
		}
		else {
			if(strcmp(buffer,"")!=0) {
				string str = buffer;
				TrimSpaces(str);
				cout<<"SERVER GOT: "<<str;
				processMessage(buffer);
			}
		}
	}

	//Close the connected socket
	if ( close(conn_s) < 0 ) {
		fprintf(stderr, "Error calling close()\n");
		exit(EXIT_FAILURE);
	}
	else {
		//closed port update
		pthread_mutex_lock(&endLock);
		numPortsClosed++;
		cout<<"Port: "<<conn_s<<" closed. It's server port."<<endl;
		cout<<"total ports closed: "<<numPortsClosed<<"/"<<numPorts<<endl;

		//if enough ports closed, tell main thread to die
		if(numPortsClosed==numPorts) {
			pthread_cond_signal(&endCV);
		}

		pthread_mutex_unlock(&endLock);
	}
}


/**
 * Creates a server socket conection.
 *
 * @param port The port to connect on.
 * @return The connection object
 */
Connection* createAServerConnection(int port) {

	int       list_s;                //listening socket
	struct    sockaddr_in servaddr;  //socket address structure
	Connection* rtn;

	//Create the listening socket
	if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "Error creating listening socket.\n");
		exit(EXIT_FAILURE);
	}

	//Set all bytes in socket address structure to
	//zero, and fill in the relevant data members
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	//Bind our socket addresss to the listening socket, and call listen()
	if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		fprintf(stderr, "Error calling bind()\n");
		exit(EXIT_FAILURE);
	}

	if ( listen(list_s, LISTENQ) < 0 ) {
		fprintf(stderr, "Error calling listen()\n");
		exit(EXIT_FAILURE);
	}

	rtn = new Connection();
	rtn->list_s = list_s;
	rtn->servaddr = servaddr;
	rtn->port = port;
	cout<<"Opened connection on port "<<port<<endl;
	return rtn;
}

/**
 * Reconstructs a reward table based on a received table
 * string from a message.
 *
 * @param width The width of the table
 * @param height The height of the table
 * @param tableStr The string to reconstruct from
 * @return The reconstructed table
 */
int** reconstructRewardTable(int width, int height, char* tableStr) {

	//create table
	int** rtn = (int**) malloc(width * sizeof(int*));
	for(int x=0; x < width; x++) {
		rtn[x] = (int*) malloc(height * sizeof(int));
	}
	cout<<"Done arr create:"<<width<<" "<<height<<endl;

	//check arr integrity
	for(int x=0; x < width; x++) {
		for(int y=0; y < height; y++) {
			cout<<"INTEGRITY: "<<rtn[x][y]<<endl;
		}
	}

	//populate table
	char* context;
	strtok_r(tableStr, ",", &context);
	int ctr=0;
	for(int x=0; x < width; x++) {
		for(int y=0; y < height; y++) {
			char* tok  = strtok_r(NULL, ",", &context);
			cout<<"TOK: "<<tok<<endl;
			int val = atoi(tok);
			rtn[x][y] = val;
			cout<<ctr<<endl;
			ctr++;
		}
	}
	cout<<"Done populating values"<<endl;
	cout<<"RETURNING"<<endl;
	return rtn;
}

/**
 * Message processing function. Takes in a message, parses it, and executes
 * necessary actions. This includes dealing with synchronization issues.
 *
 * @param message The received message
 */
void processMessage(char* message) {
	cout<<"Processing this message here: "<<message;

	if(message[0]=='R') {

		pthread_mutex_lock(&DCOPThreadLock_start);

		//increment count
		numReadyToGo++;


		//if enough people, start DCOPThread
		int numNeighbors = getNeighbors(myRobotID)->size();
		cout<<"NumReadyToGo:" <<numReadyToGo<<" out of "<<numNeighbors<<endl;

		if(numReadyToGo>=numNeighbors && !DCOPStarted) {
			DCOPThread = new pthread_t();

			if(strchr(DCOP_ALGORITHM, '1')!=NULL){
				if((pthread_create(DCOPThread, NULL, &DCOPThreadFunc_k1, (void*) myRobotID))!=0) {
					cout<<"Failure on setting up DCOP thread. "<<endl;
					exit(0);
				}
			}
			else {
				if((pthread_create(DCOPThread, NULL, &DCOPThreadFunc_k2, (void*) myRobotID))!=0) {
					cout<<"Failure on setting up DCOP thread. "<<endl;
					exit(0);
				}
			}
			DCOPStarted = true;
		}

		//go if all are ready
		if(numReadyToGo==numNeighbors+1) {
			cout<<"Signaled Ready To Go"<<endl;
			pthread_cond_signal(&DCOPCV_start);
		}

		pthread_mutex_unlock(&DCOPThreadLock_start);
	}
	else if(message[0]=='P')  {

		cout<<"About to parse this message: "<<message<<endl;

		//parse out values
		char* context;
		strtok_r(message, ":", &context);
		char* ts = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		char* valueStr = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		int value = atoi(valueStr);
		Round* round = &rounds[roundNumber];

		pthread_mutex_lock(&round->preLock);

		//store value of neighbor in preround arr
		round->preVals[nodeNumber] = value;

		//update count
		round->numPreValsReceived++;
		cout<<"PREVALS: "<<round->numPreValsReceived<<endl;

		//if enough people, signal DCOPThread to proceed
		int numNeighbors = getNeighbors(myRobotID)->size();
		if(round->numPreValsReceived==numNeighbors) {

			//do signal
			round->preSignalVal = 1;
			pthread_cond_signal(&round->preCV);
		}

		pthread_mutex_unlock(&round->preLock);
	}
	else if(message[0]=='B') {

		//parse out values
		cout<<"About to parse this message: "<<message<<endl;

		char* context;
		strtok_r(message, ":",&context);
		char* timeStampStr = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		char* bidStr = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		int bid = atoi(bidStr);		Round* round = &rounds[roundNumber];

		pthread_mutex_lock(&round->bidsLock);

		//store value of neighbor in bid arr
		round->bids[nodeNumber] = bid;

		//update count
		round->numBidsReceived++;
		cout<<"BIDS REC: "<<round->numBidsReceived<<endl;

		//if enough people, signal DCOPThread to proceed
		int numNeighbors = getNeighbors(myRobotID)->size();
		if(round->numBidsReceived==numNeighbors) {

			//do signal
			cout<<"SIGNALED BID"<<endl;
			round->bidsSignalVal = 1;
			pthread_cond_signal(&round->bidsCV);
		}
		pthread_mutex_unlock(&round->bidsLock);
	}
	else if(message[0]=='M') {
		char* context;
		strtok_r(message, ":", &context);
		char* ts = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		Round* round = &rounds[roundNumber];

		pthread_mutex_lock(&round->movedLock);

		//update counter
		if(round->expectedToMove[myRobotID]==nodeNumber || round->expectedToMove2[myRobotID]==nodeNumber)
			round->numRobotsMoved++;
		cout<<"NUM_ROBOTS_MOVED: "<<round->numRobotsMoved<<endl;
		cout<<"NUM_EXPECTED_TO_MOVE: "<<round->numExpectedToMove<<endl;

		//signal if necessary
		if(round->numRobotsMoved==round->numExpectedToMove) {
			round->movedSignalVal = 1;
			cout<<"SIGNALED TO MOVE ON"<<endl;
			pthread_cond_signal(&round->movedCV);
		}

		pthread_mutex_unlock(&round->movedLock);
	}
	else if(message[0]=='E') {
		//signal DCOPThread to proceed
		char* context;
		strtok_r(message, ":", &context);
		char* ts = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		char* expectedToMoveStr = strtok_r(NULL, ":", &context);
		char* expectedToMove2Str = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		int expectedToMoveInt = atoi(expectedToMoveStr);
		int expectedToMove2Int = atoi(expectedToMove2Str);

		Round* round = &rounds[roundNumber];

		pthread_mutex_lock(&round->expectedLock);

		//store value in expected matrix
		round->expectedToMove[nodeNumber] = expectedToMoveInt;
		round->expectedToMove2[nodeNumber] = expectedToMove2Int;

		//update numExpectedReceived
		round->numExpectedReceived++;

		//signal if necessary
		int numNeighbors = getNeighbors(myRobotID)->size();

		if(round->numExpectedReceived==numNeighbors) {
			round->expectedSignalVal = 1;
			pthread_cond_signal(&round->expectedCV);
		}

		pthread_mutex_unlock(&round->expectedLock);
	}
	else if(message[0]=='F') {
		char* context;
		strtok_r(message, ":", &context);
		char* ts = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		char* pairPosStr = strtok_r(NULL, ":", &context);
		char* pairNeighborPosStr = strtok_r(NULL, ":", &context);
		char* pairRewardStr = strtok_r(NULL, ":", &context);
		char* pairNeighborIDStr = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		int pairPos = atoi(pairPosStr);
		int pairNeighborPos = atoi(pairNeighborPosStr);
		int pairReward = atoi(pairRewardStr);
		int pairNeighborID = atoi(pairNeighborIDStr);

		//store first bid
		cout<<"Storing first bid in table."<<endl;
		Round* round = &rounds[roundNumber];
		K2Bid* pairStorage = &round->firstBids[nodeNumber];
		cout<<"Pair storage aint it."<<endl;
		pairStorage->myPos = pairPos;
		pairStorage->neighborPos = pairNeighborPos;
		pairStorage->reward =  pairReward;
		pairStorage->neighborID = pairNeighborID;
		cout<<"DONE w/ store."<<endl;

		//update first bid count
		pthread_mutex_lock(&round->firstBidsLock);
		round->numFirstBidsReceived++;
		cout<<"First bid count: "<<round->numFirstBidsReceived<<endl;
		if(round->numFirstBidsReceived==getNeighbors(myRobotID)->size()) {
			round->firstBidsSignal =  1;
			pthread_cond_signal(&round->firstBidsCV);
		}
		pthread_mutex_unlock(&round->firstBidsLock);
		cout<<"done w/ first bid."<<endl;
	}
	else if(message[0]=='S') {
		char* context;
		strtok_r(message, ":", &context);
		char* ts = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		char* pairPosStr = strtok_r(NULL, ":", &context);
		char* pairNeighborPosStr = strtok_r(NULL, ":", &context);
		char* pairRewardStr = strtok_r(NULL, ":", &context);
		char* pairNeighborIDStr = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		int pairPos = atoi(pairPosStr);
		int pairNeighborPos = atoi(pairNeighborPosStr);
		int pairReward = atoi(pairRewardStr);
		int pairNeighborID = atoi(pairNeighborIDStr);

		//store first bid
		Round* round = &rounds[roundNumber];
		K2Bid* pairStorage = &round->secondBids[nodeNumber];
		pairStorage->myPos = pairPos;
		pairStorage->neighborPos = pairNeighborPos;
		pairStorage->reward =  pairReward;
		pairStorage->neighborID = pairNeighborID;

		//update first bid count
		pthread_mutex_lock(&round->secondBidsLock);
		round->numSecondBidsReceived++;
		if(round->numSecondBidsReceived==getNeighbors(myRobotID)->size()) {
			round->secondBidsSignal =  1;
			pthread_cond_signal(&round->secondBidsCV);
		}
		pthread_mutex_unlock(&round->secondBidsLock);
	}
	else if(message[0]=='T') {
		char* context;
		strtok_r(message, ":", &context);
		char* ts = strtok_r(NULL, ":", &context);
		char* nodeNumberStr = strtok_r(NULL, ":", &context);
		char* roundNumberStr = strtok_r(NULL, ":", &context);
		char* pairPosStr = strtok_r(NULL, ":", &context);
		char* pairNeighborPosStr = strtok_r(NULL, ":", &context);
		char* pairRewardStr = strtok_r(NULL, ":", &context);
		char* pairNeighborIDStr = strtok_r(NULL, ":", &context);
		int nodeNumber = atoi(nodeNumberStr);
		int roundNumber = atoi(roundNumberStr);
		int pairPos = atoi(pairPosStr);
		int pairNeighborPos = atoi(pairNeighborPosStr);
		int pairReward = atoi(pairRewardStr);
		int pairNeighborID = atoi(pairNeighborIDStr);

		//store third bid
		Round* round = &rounds[roundNumber];
		K2Bid* pairStorage = &round->thirdBids[nodeNumber];
		pairStorage->myPos = pairPos;
		pairStorage->neighborPos = pairNeighborPos;
		pairStorage->reward =  pairReward;
		pairStorage->neighborID = pairNeighborID;

		//update third bid count
		pthread_mutex_lock(&round->thirdBidsLock);
		round->numThirdBidsReceived++;
		if(round->numThirdBidsReceived==getNeighbors(myRobotID)->size()) {
			round->thirdBidsSignal =  1;
			pthread_cond_signal(&round->thirdBidsCV);
		}
		pthread_mutex_unlock(&round->thirdBidsLock);
	}
	else {
		cout<<"MESSAGE MATCH FAILURE"<<endl;
	}
}

/**
 * The DCOP thread function for k=2 algorithms. This function is
 * commanly used for all k=2 algorithms as it executes the basic
 * DCOP steps.
 */
void* DCOPThreadFunc_k2(void* arg) {

	//wait until start processing has been fully completed
	//main function is to make sure that all nodes are up
	//before start the DCOP thread behavior
	cout<<"DCOP THREAD STARTED"<<endl;
	pthread_mutex_lock(&DCOPThreadLock_start);
	if(numReadyToGo!=getNeighbors(myRobotID)->size()+1) {
		cout<<"Waiting for other agents."<<endl;
		pthread_cond_wait(&DCOPCV_start, &DCOPThreadLock_start);
	}
	pthread_mutex_unlock(&DCOPThreadLock_start);

	cout<<"About to enter loop."<<endl;
	while(numRoundsElapsed  < NUM_ROUNDS) {
		//round variables - ease of use
		Round*  round = &rounds[numRoundsElapsed];
		cout<<"NUM ROUND ELAPSED: "<<numRoundsElapsed<<endl;

		//GET PREROUND VALUE
		int myPrior = computeMyPreroundValue();

		//send preround value
		vector<int>* neighbors = getNeighbors(myRobotID);
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			sendPreroundValueMsg(connections[neighbor],numRoundsElapsed, myPrior);
		}
		cout<<"Should have sent preround."<<endl;

		//wait for all preround values from others
		pthread_mutex_lock(&round->preLock);
		if(round->preSignalVal==0) {
			cout<<"Waiting for others preround."<<endl;
			pthread_cond_wait(&round->preCV, &round->preLock);
		}
		pthread_mutex_unlock(&round->preLock);

		//compute local bid (first bid)
		updateSignalStrengths(round->neighborSS, neighbors->size());
		int myLocalBid = computeGain(DCOP_ALGORITHM, round->neighborSS, neighbors->size());
		K2Bid* firstBid = &(round->firstBids[myRobotID]);
		firstBid->myPos = round->ifIWinMyLoc;
		firstBid->neighborPos = -1;
		firstBid->reward = myLocalBid;
		firstBid->neighborID = -1;
		cout<<"MY FIRST BID FOR ROUND "<<numRoundsElapsed<<" :"<<myLocalBid<<endl;

		//send first bid
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			sendFirstBid(connections[neighbor], numRoundsElapsed, firstBid);
		}

		//wait for first bids
		pthread_mutex_lock(&round->firstBidsLock);
		if(round->firstBidsSignal==0) {
			cout<<"Waiting for first bids."<<endl;
			pthread_cond_wait(&round->firstBidsCV, &round->firstBidsLock);
		}
		pthread_mutex_unlock(&round->firstBidsLock);
		cout<<"should have received all first bids."<<endl;

		//find max that is not my own
		int max=INT_MIN, maxID=-1, maxPos=-1;
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			if(round->firstBids[neighbor].reward > max || (round->firstBids[neighbor].reward==max && neighbor < maxID)) {
				max = round->firstBids[neighbor].reward;
				maxID = neighbor;
				maxPos = round->firstBids[neighbor].myPos;
			}
		}
		cout<<"MAX ID Belongs to "<<maxID<<" for first bid "<<max<<" in round "<<numRoundsElapsed<<endl;

		//compute joint gain and send out pairing request
		int jointGain = myLocalBid + max - round->neighborSS[maxID];
		K2Bid* secondBid = &(round->secondBids[myRobotID]);
		secondBid->myPos = round->ifIWinMyLoc;
		secondBid->neighborPos = maxPos;
		secondBid->reward = jointGain;
		secondBid->neighborID = maxID;

		//send second bid
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			sendSecondBid(connections[neighbor], numRoundsElapsed, secondBid);
		}

		//receive second bids
		pthread_mutex_lock(&round->secondBidsLock);
		if(round->secondBidsSignal==0) {
			cout<<"Waiting for second bids."<<endl;
			pthread_cond_wait(&round->secondBidsCV, &round->secondBidsLock);
		}
		pthread_mutex_unlock(&round->secondBidsLock);
		cout<<"should have received all second bids."<<endl;

		//Third bid: If who I wanted to pair with wants to pair with me, do that. Else bid my local bid.
		K2Bid* thirdBidComp = computeThirdBid(round, secondBid, myLocalBid);
		K2Bid* thirdBid = &(round->thirdBids[myRobotID]);
		thirdBid->myPos = thirdBidComp->myPos;
		thirdBid->neighborPos = thirdBidComp->neighborPos;
		thirdBid->reward = thirdBidComp->reward;
		thirdBid->neighborID = thirdBidComp->neighborID;

		//send third bid
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			sendThirdBid(connections[neighbor], numRoundsElapsed, thirdBid);
		}

		//wait for all 3rd bids
		pthread_mutex_lock(&round->thirdBidsLock);
		if(round->thirdBidsSignal==0) {
			cout<<"Waiting for third bids."<<endl;
			pthread_cond_wait(&round->thirdBidsCV, &round->thirdBidsLock);
		}
		pthread_mutex_unlock(&round->thirdBidsLock);
		cout<<"should have received all third bids."<<endl;

		//compute best bid
		K2Bid* bestBid = &(round->thirdBids[myRobotID]);
		int bestBidID = myRobotID;
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			K2Bid* bid = &round->thirdBids[neighbor];
			cout<<"round "<<numRoundsElapsed<<" bid "<<neighbor<<" :"<<bid->reward<<endl;
			if(bid->reward > bestBid->reward) {
				bestBid = bid;
				bestBidID =  neighbor;
			}
		}
		round->expectedToMove[myRobotID] = bestBidID;
		round->expectedToMove2[myRobotID] = bestBid->neighborID;

		cout<<"MAX BID belongs to "<<bestBidID<<" and "<<bestBid->neighborID<<" for "<<bestBid->reward<<endl;

		//compute number to move
		pthread_mutex_lock(&round->movedLock);
		round->numExpectedToMove=0;
		if(isNeighbor(bestBidID))
			round->numExpectedToMove++;
		if(isNeighbor(bestBid->neighborID))
			round->numExpectedToMove++;
		cout<<"I expect "<<round->numExpectedToMove<<" DM msgs."<<endl;
		pthread_mutex_unlock(&round->movedLock);

		//send who I expect to move to neighbors
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor =  neighbors->at(x);
			sendExpectedToMoveMsg(connections[neighbor], numRoundsElapsed, bestBidID, bestBid->neighborID);
		}

		//wait for movement expectations from neighbors
		pthread_mutex_lock(&round->expectedLock);
		if(round->expectedSignalVal==0) {
			cout<<"Waiting for other expected values."<<endl;
			pthread_cond_wait(&round->expectedCV, &round->expectedLock);
		}
		pthread_mutex_unlock(&round->expectedLock);

		//compute move or not move
		if(bestBidID==myRobotID) {
			cout<<"Guess it's my turn to move."<<endl;
			if(bestBid->neighborID!=-1)
				cout<<"My bro "<<bestBid->neighborID<<" should also move."<<endl;

			//move
			move(DCOP_ALGORITHM);
			sleep(5);

			//if i have a bro moving, send done moving to my bro.
			if(bestBid->neighborID!=-1) {
				sendDoneMovingMsg(connections[bestBid->neighborID], numRoundsElapsed);
			}

			//if i have a bro moving, wait for a done message from them.
			if(bestBid->neighborID!=-1) {
				pthread_mutex_lock(&round->movedLock);
				if(round->movedSignalVal==0) {
					cout<<"waiting for signal from "<<round->expectedToMove2[myRobotID]<<endl;
					pthread_cond_wait(&round->movedCV, &round->movedLock);
				}
				pthread_mutex_unlock(&round->movedLock);
			}

			//send done moving to all my neighbors who aren't my bro who expect me to move.
			for(int x=0; x < neighbors->size(); x++) {
				int neighbor =  neighbors->at(x);

				//if neighbor is my bro, don't do it.
				if(neighbor==bestBid->neighborID)
					continue;

				//do it
				if(round->expectedToMove[neighbor]==myRobotID || round->expectedToMove2[neighbor]==myRobotID) {
					sendDoneMovingMsg(connections[neighbor], numRoundsElapsed);
				}
			}
		}
		else if(bestBid->neighborID==myRobotID)  {
			cout<<"It's my turn to move."<<endl;
			cout<<"My bro "<<bestBidID<<" won me the bid."<<endl;

			//move
			move(DCOP_ALGORITHM);

			//if i have a bro moving, send done moving to my bro.
			if(bestBidID!=-1) {
				sendDoneMovingMsg(connections[bestBidID], numRoundsElapsed);
			}

			//if i have a bro moving, wait for a done message from them.
			if(bestBidID!=-1) {
				cout<<"Not my turn."<<endl;
				pthread_mutex_lock(&round->movedLock);
				if(round->movedSignalVal==0) {
					cout<<"waiting for signal from "<<round->expectedToMove[myRobotID]<<endl;
					pthread_cond_wait(&round->movedCV, &round->movedLock);
				}
				pthread_mutex_unlock(&round->movedLock);
			}

			//send done moving to all who expect me to move who are not my bro.
			for(int x=0; x < neighbors->size(); x++) {
				int neighbor =  neighbors->at(x);

				//if its my bro, dont do it.
				if(neighbor==bestBidID)
					continue;

				//else do it
				if(round->expectedToMove[neighbor]==myRobotID || round->expectedToMove2[neighbor]==myRobotID) {
					sendDoneMovingMsg(connections[neighbor], numRoundsElapsed);
				}
			}
		}
		else {
			cout<<"Not my turn to move."<<endl;

			//wait for all to move
			pthread_mutex_lock(&round->movedLock);
			if(round->movedSignalVal==0) {
				cout<<"Waiting for winner(s) to move."<<endl;
				pthread_cond_wait(&round->movedCV, &round->movedLock);
			}
			pthread_mutex_unlock(&round->movedLock);

			//send done moving to all who expect me to move
			for(int x=0; x < neighbors->size(); x++) {
				int neighbor =  neighbors->at(x);

				//else do it
				if(round->expectedToMove[neighbor]==myRobotID || round->expectedToMove2[neighbor]==myRobotID) {
					sendDoneMovingMsg(connections[neighbor], numRoundsElapsed);
				}
			}
		}

		cout<<"ROUND "<<numRoundsElapsed<<" DONE."<<endl;

		//increment round number
		numRoundsElapsed++;

	}
}

/**
 * The DCOP thread function for k=1 algorithms. This function is
 * commanly used for all k=1 algorithms as it executes the basic
 * DCOP steps.
 */
void* DCOPThreadFunc_k1(void* arg) {

	//wait until start processing has been fully completed
	//main function is to make sure that all nodes are up
	//before start the DCOP thread behavior
	cout<<"DCOP THREAD STARTED"<<endl;
	pthread_mutex_lock(&DCOPThreadLock_start);
	if(numReadyToGo!=getNeighbors(myRobotID)->size()+1) {
		cout<<"Waiting for other agents."<<endl;
		pthread_cond_wait(&DCOPCV_start, &DCOPThreadLock_start);
	}
	pthread_mutex_unlock(&DCOPThreadLock_start);

	cout<<"About to enter loop."<<endl;
	while(numRoundsElapsed < NUM_ROUNDS) {

		//round variables - ease of use
		Round*  round = &rounds[numRoundsElapsed];
		cout<<"NUM ROUND ELAPSED: "<<numRoundsElapsed<<endl;

		//GET PREROUND VALUE
		int myPrior = computeMyPreroundValue();

		//send preround value
		vector<int>* neighbors = getNeighbors(myRobotID);
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			sendPreroundValueMsg(connections[neighbor],numRoundsElapsed, myPrior);
		}
		cout<<"Should have sent preround."<<endl;

		//wait for all preround values from others
		pthread_mutex_lock(&round->preLock);
		if(round->preSignalVal==0) {
			cout<<"Waiting for others preround."<<endl;
			pthread_cond_wait(&round->preCV, &round->preLock);
		}
		pthread_mutex_unlock(&round->preLock);

		//COMPUTE MY BID
		updateSignalStrengths(round->neighborSS, neighbors->size());
		int myBid = computeGain(DCOP_ALGORITHM, round->neighborSS, neighbors->size());
		cout<<"MY BID FOR ROUND "<<numRoundsElapsed<<" :"<<myBid<<endl;
		round->bids[myRobotID] = myBid;

		//send my signal str to vis server
//		sendCumSignalStrMsg(NULL, numRoundsElapsed, computeCumSignalStr(round->neighborSS, NUM_AGENTS-1));
//		for(int x=0; x < NUM_AGENTS; x++) {
//			if(x!=myRobotID) {
//				sendInstSignalStrMsg(NULL, x, numRoundsElapsed, round->neighborSS[x]);
//			}
//		}

		//send my bid
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor =  neighbors->at(x);
			sendRoundBidMsg(connections[neighbor], numRoundsElapsed, myBid);
		}
		cout<<"Should have sent my bids."<<endl;

		//wait for all bids
		pthread_mutex_lock(&round->bidsLock);
		if(round->bidsSignalVal==0) {
			cout<<"Waiting for other bids"<<endl;
			pthread_cond_wait(&round->bidsCV, &round->bidsLock);
		}
		pthread_mutex_unlock(&round->bidsLock);

		//find max
		int max=round->bids[myRobotID], maxID=myRobotID;
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor = neighbors->at(x);
			if(round->bids[neighbor] > max || (round->bids[neighbor]==max && neighbor < maxID)) {
				max = round->bids[neighbor];
				maxID = neighbor;
			}
		}
		round->expectedToMove[myRobotID] = maxID;

		cout<<"MAX ID Belongs to "<<maxID<<" for bid "<<max<<" in round "<<numRoundsElapsed<<endl;

		//compute number to move
		pthread_mutex_lock(&round->movedLock);
		round->numExpectedToMove = 1;
		pthread_mutex_unlock(&round->movedLock);

		//send who I expect to move for neighbors
		for(int x=0; x < neighbors->size(); x++) {
			int neighbor =  neighbors->at(x);
			sendExpectedToMoveMsg(connections[neighbor], numRoundsElapsed, maxID, -1);
		}

		//wait for movement expectations from neighbors
		pthread_mutex_lock(&round->expectedLock);
		if(round->expectedSignalVal==0) {
			cout<<"Waiting for other expected values."<<endl;
			pthread_cond_wait(&round->expectedCV, &round->expectedLock);
		}
		pthread_mutex_unlock(&round->expectedLock);

		//Move or not move code
		if(maxID==myRobotID) {

			//if its my turn to move, move and send done moving to all who expect me to move.
			cout<<"I guess its my turn to move. "<<endl;

			//move
			int newPos = move(DCOP_ALGORITHM);
			cout<<"Moved to new position: "<<newPos<<endl;

			//send done moving to all who expect me to move.
			for(int x=0; x < neighbors->size(); x++) {
				int neighbor =  neighbors->at(x);
				if(round->expectedToMove[neighbor]==myRobotID) {
					sendDoneMovingMsg(connections[neighbor], numRoundsElapsed);
				}
			}
		}
		else {
			//wait for done moving message from person who I expect to move.
			cout<<"Not my turn."<<endl;
			pthread_mutex_lock(&round->movedLock);
			if(round->movedSignalVal==0) {
				cout<<"waiting for signal from "<<round->expectedToMove[myRobotID]<<endl;
				pthread_cond_wait(&round->movedCV, &round->movedLock);
			}
			pthread_mutex_unlock(&round->movedLock);

			//send done moving to all who expect me to move.
			for(int x=0; x < neighbors->size(); x++) {
				int neighbor =  neighbors->at(x);
				if(round->expectedToMove[neighbor]==myRobotID) {
					sendDoneMovingMsg(connections[neighbor], numRoundsElapsed);
				}
			}
		}

		cout<<"ROUND "<<numRoundsElapsed<<" DONE."<<endl;

		//increment round number
		numRoundsElapsed++;
	}
}