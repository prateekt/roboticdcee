//M:TS:ID:RN
//B:TS:ID:RN:Bid
//P:TS:ID:RN:VAL
//R:TS:ID
//C:TS:ID:RN:VAL
//I:TS:ID:RN:WITH:VAL

#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>       //  socket definitions
#include <sys/types.h>        //  socket types
#include <arpa/inet.h>        //  inet (3) funtions
#include <unistd.h>           //  misc. UNIX functions
#include "helper.h"           //  our own helper functions
#include "dcop.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <map>
#include "structs.h"
#include <vector>

using namespace std;


/**
 -----
 * This file contains a lot of code!
 -----
 */


//Global constants
#define ROBOT_NODE_PORT       (200002)
#define VIS_SERVER_PORT       (1960)
#define MAX_MESSAGE_SIZE      (500)
#define VIS_SERVER_IP	      ("192.168.5.100")
#define VIS_SERVER_ON	      (true)
#define VIS_SERVER_OFF	      (false)

//function prototypes
void initDCOPSynch();
int getTimeStamp();
int startAServer(int port, int otherID);
void* serverThreadFunc(void* arg);
void* DCOPThreadFunc_k1(void* arg);
void* DCOPThreadFunc_k2(void* arg);

Connection* createAServerConnection(int port);
Connection* connectToNode(string remoteAddr, int port);
void initiateConnectionToNode(string remoteAddr, int remotePort, int myServerPort, int otherRID);
int sendMessage(Connection* conn, string message);
void processMessage(char* message);

//function prototypes for messages - DCOP Messages
int sendReadyToStartMsg(Connection* conn);
int sendPreroundValueMsg(Connection* conn, int value);
int sendRoundBidMsg(Connection* conn, int roundNumber, int bid);
int sendDoneMovingMsg(Connection* conn, int roundNumber);
int sendNotMovingMsg(Connection* conn, int roundNumber);
int sendRewardTable(Connection* conn, int roundNumber, int width, int height, int** table);
int sendfirstBid(Connection* conn, int roundNumber, K2Bid* pair);
int sendSecondBid(Connection* conn, int roundNumber, K2Bid* pair);
int sendThirdBid(Connection* conn, int roundNumber, K2Bid* pair);

//function prototypes for messages -- VIS SERVER
int sendCumSignalStrMsg(Connection* conn, int roundNumber, int value);
int sendInstSignalStrMsg(Connection* conn, int with, int roundNumber, int value);

//function prototypes
int computeMyPreroundValue();
int computeGain(int* neighborVals, int numNeighbors);
void updateSignalStrengths(int* neighbors, int numNeighbors);
int** reconstructRewardTable(int width, int height, char* tableStr);
void closeConnections();
void trim(char *s);

//Global variables
extern Connection* connections[NUM_AGENTS];
extern Connection* myServers[NUM_AGENTS];
extern pthread_t* myServerThread;
extern Connection* visServerConn;
extern bool visServerOn;

//Network Message Management global variables
extern pthread_t* DCOPThread;

//start sync
extern bool DCOPStarted;
extern int numReadyToGo;
extern pthread_cond_t DCOPCV_start;
extern pthread_mutex_t DCOPThreadLock_start;

//round sync vars
extern Round rounds[NUM_ROUNDS];

//end sync
extern int numPorts;
extern int numPortsClosed;
extern pthread_cond_t endCV;
extern pthread_mutex_t endLock;

//maps
extern map<int,string>* robotIDToIP;
extern map<string,int>* hwAddrToID;
extern map<int,vector<int>*>* neighborMap;

//map functions
string getIP(int rid);
int getID(string hw);
vector<int>* getNeighbors(int dcopID);
vector<int>* getNeighbors_chain(int dcopID);
vector<int>* getNeighbors_full(int dcopID);
bool isNeighbor(int dude);

#endif
