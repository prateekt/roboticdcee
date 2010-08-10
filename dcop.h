#ifndef DCOP_H
#define DCOP_H

#include <string>
#include <map>
#include "structs.h"

using namespace std;

/**
 -----
 * Contains implementation of DCOP algorithms. There are
 * algorithm-specific functions for gain computation, bid computation, move computation, etc.
 -----
 */

//function prototypes
int computeMyPreroundValue();
int computeGain(string algo, int* neighborVals, int numNeighbors);
void updateSignalStrengths(int* neighbors, int numNeighbors);
int computeCumSignalStr(int* newRound, int numNeighbors);
int move(string algo);
int executeSampling(string algo);
int getUnexploredCost(string algo);

//algo specific bid functions
int computeGain_MPGM1(int* neighborVals, int numNeighbors);
int computeGain_MGMMean1(int* neighborVals, int numNeighbors);
int computeGain_BeRebid1(int* neighborVals, int numNeighbors);
int computeGain_MGMMean2(int* neighborVals, int numNeighbors);
int computeGain_MPGM2(int* neighborVals, int numNeighbors);
int computeGain_BeRebid2(int* neighborVals, int numNeighbors);
int computeGain_SEI1(int* neighborVals, int numNeighbors);
int computeGain_SEI2(int* neighborVals, int numNeighbors);
K2Bid* computeThirdBid(Round* round, K2Bid* secondBid, int myLocalBid);

//algo specific move functions
int move_MPGM1();
int move_MGMMean1();
int move_BeRebid1();
int move_MPGM2();
int move_BeRebid2();
int move_MGMMean2();
int move_SEI1();
int move_SEI2();
int move_sample();
int standardForwardMovement();

//algo specific helpers - berebid1
bool neighborsStatic(int* current, int* prior, int numNeighbors);
map<int,int>* genHistogram(int*  samples, int numSamples);
double computeMean(int* samples, int numSamples);
double computeStdDev(int* samples, int numSamples);
double computeTerm3(int ts, double rb);
double computeTerm2(int* samples, int numSamples, double rb, int te);
double computeGaussian(double x, double u, double o);
double compute_P(double x, double te);
double compute_f(double x);
double compute_F(double x);

//DCOP Global Variables
extern int myRobotID;
extern int myLoc;
extern int numRoundsElapsed;

#endif