#ifndef STRUCTS_H
#define STRUCTS_H

#include <sys/socket.h>       //  socket definitions
#include <sys/types.h>        //  socket types
#include <arpa/inet.h>        //  inet (3) funtions
#include <unistd.h>           //  misc. UNIX functions
#include "config.h"

//structs
struct Rbest {
	int neighborPos[NUM_AGENTS];
	int signalStr;
};

struct K2Bid {
	int myPos;
	int neighborPos;
	int reward;
	int neighborID;
};

//Connection struct
typedef  struct {
	int list_s;                //listening socket
	int conn_s;
	struct sockaddr_in servaddr;  //socket address structure
	int port;
	int robotID;
} Connection;

struct Round {

	//synch
	pthread_mutex_t preLock;
	pthread_cond_t preCV;
	int preSignalVal;
	pthread_mutex_t bidsLock;
	pthread_cond_t bidsCV;
	int bidsSignalVal;
	pthread_mutex_t movedLock;
	pthread_cond_t movedCV;
	int movedSignalVal;
	int numRobotsMoved;
	pthread_mutex_t expectedLock;
	pthread_cond_t expectedCV;
	int expectedSignalVal;

	//DCOP vars
	int neighborSS[NUM_AGENTS];
	int preVals[NUM_AGENTS];
	int numPreValsReceived;
	int bids[NUM_AGENTS];
	int numBidsReceived;
	int ifIWinMyLoc;
	int expectedToMove[NUM_AGENTS];  //agent id to who the agent expects to move.
	int expectedToMove2[NUM_AGENTS]; //agent id to who the agent expects to move, in case two agents are moving.
	int numExpectedReceived;
	int numExpectedToMove;

	//k2
	int neighborMatrices[NUM_AGENTS][NUM_AGENTS][NUM_ROUNDS]; //for ea neighbor you have -- nid by pos, contains R
	pthread_mutex_t neighborMatricesLock;
	pthread_cond_t neighborMatricesCV;
	int numNeighborMatricesReceived;
	int neighborMatricesSignal;

	//k2 -- first bids
	K2Bid firstBids[NUM_AGENTS];
	pthread_mutex_t firstBidsLock;
	pthread_cond_t firstBidsCV;
	int numFirstBidsReceived;
	int firstBidsSignal;

	//k2 -- second bids
	K2Bid secondBids[NUM_AGENTS];
	pthread_mutex_t secondBidsLock;
	pthread_cond_t secondBidsCV;
	int numSecondBidsReceived;
	int secondBidsSignal;

	//k2 -- third bids
	K2Bid thirdBids[NUM_AGENTS];
	pthread_mutex_t thirdBidsLock;
	pthread_cond_t thirdBidsCV;
	int numThirdBidsReceived;
	int thirdBidsSignal;
};

#endif
