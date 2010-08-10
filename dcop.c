#include "dcop.h"
#include <stdlib.h>
#include "iwspy.h"
#include <limits.h>
#include "create_lib.h"
#include "sampler.h"
#include "Network.h"
#include <pthread.h>

using namespace std;

/**
 -----
 * Contains implementation of DCOP algorithms. There are
 * algorithm-specific functions for gain computation, bid computation, move computation, etc.
 -----
 */

//DCOP simulation vars
int numRoundsElapsed = 0; //number of rounds elapsed

//DCOP variables for me
int myRobotID; //my robot id
int myLoc = 0; //my current location in the environment
int cumSignalStr = 0; //my current cumulative signal strength

//DCOP Vars for me - BeRebid
Rbest* rbest; //The best configuration I have encountered and might backtrack to in BE algorithms

/**
 * Computes a third bid based on k=2 algorithm.
 *
 * @param round The current round struct
 * @param bid The second bid I made
 * @param The current local gain
 * @return The third bid
 */
K2Bid* computeThirdBid(Round* round, K2Bid* bid, int localGain) {
	K2Bid* rtn = new K2Bid();
	if(round->secondBids[bid->neighborID].neighborID==myRobotID) {
		//my chosen neighbor wants to
		//pair with me so bid stays same
		rtn->neighborID = bid->neighborID;
		rtn->reward = bid->reward;
		rtn->myPos = bid->myPos;
		rtn->neighborPos = bid->neighborPos;
	}
	else {
		//this pairing aint gonna work
		rtn->neighborID = -1;
		rtn->reward = localGain;
		rtn->myPos = round->ifIWinMyLoc;
		rtn->neighborPos = -1;
	}

	return bid;
}

/**
 * Updates the cumulative signal strength based
 * on a new round.
 *
 * @param newRound An array of signal strengths with neighbors for the current round.
 * @param numNeighbors The number of neighbors
 * @return The new cumulative signal strength
 */
int computeCumSignalStr(int* newRound, int numNeighbors) {
	for(int x=0; x < numNeighbors; x++) {
		if(x!=myRobotID) {
			cumSignalStr+= newRound[x];
		}
	}

	return cumSignalStr;
}

/**
 * Returns the current preround value (in DCEE, a robot's location).
 */
int computeMyPreroundValue() {
	return myLoc;
}

/**
 * Executes a sampling routine if an algorithm calls for it.
 *
 * @param algo The algorithm being used
 * @return Whether the sampling finished successfully
 */
int executeSampling(string algo) {
	if(algo.compare("BeRebid1")==0)
		getSamples(100,5,NUM_AGENTS-1);
	if(algo.compare("BeRebid2")==0)
		getSamples(100,5,NUM_AGENTS-1);
	return 1;
}

/**
 * Computes the gain based on a given algorithm.
 *
 * @param algo The algorithm being used
 * @param neighborVals The values of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain(string algo, int* neighborVals, int numNeighbors) {
	if(algo.compare("MPGM1")==0)
		return computeGain_MPGM1(neighborVals, numNeighbors);
	if(algo.compare("MGMMean1")==0)
		return computeGain_MGMMean1(neighborVals, numNeighbors);
	if(algo.compare("BeRebid1")==0)
		return computeGain_BeRebid1(neighborVals, numNeighbors);
	if(algo.compare("MPGM2")==0)
		return computeGain_MPGM2(neighborVals, numNeighbors);
	if(algo.compare("MGMMean2")==0)
		return computeGain_MGMMean2(neighborVals, numNeighbors);
	if(algo.compare("BeRebid2")==0)
		return computeGain_BeRebid2(neighborVals, numNeighbors);
	if(algo.compare("SEI1")==0)
		return computeGain_SEI1(neighborVals, numNeighbors);
	if(algo.compare("SEI2")==0)
		return computeGain_SEI2(neighborVals, numNeighbors);
	//else error
	cout<<"Algorithm: "<<algo<<" could not be supported for bidding."<<endl;
	exit(0);
}

/**
 * Berebid2 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_BeRebid2(int* neighborVals, int numNeighbors) {
	return computeGain_BeRebid1(neighborVals, numNeighbors);
}

int computeGain_MGMMean2(int* neighborVals, int numNeighbors) {
	return computeGain_MGMMean1(neighborVals, numNeighbors);
}

/**
 * MPGM2 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_MPGM2(int* neighborVals, int numNeighbors) {
	return computeGain_MPGM1(neighborVals, numNeighbors);
}

/**
 * SEI2 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_SEI2(int* neighborVals, int numNeighbors) {
	return computeGain_SEI1(neighborVals, numNeighbors);
}

/**
 * Berebid1 specific gain function. Handles updating of RBest.
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_BeRebid1(int* neighborVals, int numNeighbors) {
	//Get current round from matrix
	Round* round = &rounds[numRoundsElapsed];

	//compute sum of signal str w/ neighbors
	int sum=0;
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		sum+= neighborVals[neighbor];
	}

	//update Rbest
	if(rbest==NULL) {
		rbest = new Rbest();

		//deep copy preround values into Rbest
		for(int x=0; x < NUM_AGENTS; x++) {
			rbest->neighborPos[x] = round->preVals[x];
		}
		rbest->neighborPos[myRobotID] = myLoc;

		//set signal str
		rbest->signalStr = sum;
	}
	else {
		if(neighborsStatic(round->preVals, rbest->neighborPos, NUM_AGENTS) && rbest->signalStr > sum) {
			//do nothing to update rbest
		}
		else {
			//set Rbest to RC

			//deep copy preround values into Rbest
			for(int x=0; x < NUM_AGENTS; x++) {
				rbest->neighborPos[x] = round->preVals[x];
			}
			rbest->neighborPos[myRobotID] = myLoc;

			//set signal str
			rbest->signalStr = sum;

		}
	}

	//compute Vback
	int vback = rbest->signalStr * (NUM_ROUNDS - numRoundsElapsed);

	//compute Vexplore
	int vexplore = INT_MIN;
	for(int te=0; te < NUM_ROUNDS; te++) {
		double ts = NUM_ROUNDS - (te+1);
		double term1 = te * AVG_SIGNAL_STR;
		double term2 = computeTerm2(signalStrSamples, numSignalStrSamples, rbest->signalStr, te);
		double term3 = computeTerm3(ts,rbest->signalStr);

		int currentSum = term1 + term2 + term3;
		if(currentSum > vexplore)
			vexplore = currentSum;
	}

	//update policy storage
	if(vback > vexplore) {
		//backtrack if i win
		round->ifIWinMyLoc = rbest->neighborPos[myRobotID];

		//actually return the bid
		return vback;

	}
	else {
		//explore if i win
		round->ifIWinMyLoc = myLoc+1;

		//actually return the bid
		return vexplore;
	}
}

/**
 * Returns true whether neighbors remained static between two rounds.
 *
 * @param current Array of current neighbor positions
 * @param prior Array of neighbor positions from last round
 * @param numNeighbors The number of neighbors
 * @return Whether the neighbors remained stationary.
 */
bool neighborsStatic(int* current, int* prior, int numNeighbors) {
	for(int x=0; x < numNeighbors;x++) {
		if(current[x]!=prior[x])
			return false;
	}

	return true;
}

/**
 * MPGM1 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_MGMMean1(int* neighborVals, int numNeighbors) {
	//Get current round from matrix
	Round* round = &rounds[numRoundsElapsed];

	//easy exit
	if(numNeighbors==0) {
		return 0;
	}

	//sum signal str
	int sumOfSignalStr=0;
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		sumOfSignalStr+= neighborVals[neighbor];
	}

	//policy storage
	round->ifIWinMyLoc = myLoc + 1;

	//return according to MGM mean formula
	return numNeighbors * AVG_SIGNAL_STR - sumOfSignalStr;
}

/**
 * SEI1 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_SEI1(int* neighborVals, int numNeighbors) {
	//Get current round from matrix
	Round* round = &rounds[numRoundsElapsed];

	//easy exit
	if(numNeighbors==0) {
		return 0;
	}

	//sum signal str
	int sumOfSignalStr=0;
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		sumOfSignalStr+=neighborVals[neighbor];
	}

	int avgSignalStr = sumOfSignalStr/neighbors->size();
	if(avgSignalStr>I_PARAM)
		return INT_MIN; //bad bid

	//policy storage
	round->ifIWinMyLoc = myLoc + 1;

	//return according to SEI formula
	return numNeighbors * I_PARAM - sumOfSignalStr;
}

/**
 * MPGM1 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int computeGain_MPGM1(int* neighborVals, int numNeighbors) {

	//Get current round from matrix
	Round* round = &rounds[numRoundsElapsed];

	//easy exit
	if(numNeighbors==0) {
		return 0;
	}

	//sum signal str
	int sumOfSignalStr=0;
	for(int x=0; x < getNeighbors(myRobotID)->size(); x++) {
		int neighbor = getNeighbors(myRobotID)->at(x);
		sumOfSignalStr+= neighborVals[neighbor];
	}

	//policy storage
	round->ifIWinMyLoc = myLoc + 1;

	//return according to MPGM formula
	return numNeighbors * MAX_SIGNAL_STR - sumOfSignalStr;
}

/**
 * Updates array of signal strengths by quering IWSPY.
 *
 * @param neighbors The array to store the values of neighbor links
 * @param numNeighbors The number of neighbors
 */
void updateSignalStrengths(int* neighbors, int numNeighbors) {

	//get signal strength using IWSPY
	map<int,int>* signals = getIWSPYResultsByAgent();
	vector<int>* neighborsList = getNeighbors(myRobotID);
	for(int y=0; y < neighborsList->size(); y++) {
		int neighbor = neighborsList->at(y);
		if(signals->find(neighbor)->second > 0 && signals->find(neighbor)->second <= MAX_SIGNAL_STR) {
			neighbors[neighbor] = signals->find(neighbor)->second;
		}
	}

	//debug printout
	cout<<"SIGNAL_STR {";
	for(int x=0; x < NUM_AGENTS; x++) {
		if(x!=NUM_AGENTS-1) {
			cout<<neighbors[x]<<",";
		}
		else{
			cout<<neighbors[x];
		}
	}
	cout<<"}"<<endl;
}

/**
 * Cost function for unexplored locations.
 *
 * @param algo The algorithm
 * @return The cost of an unexplored location
 */
int getUnexploredCost(string algo) {
	if(algo.compare("MPGM1")==0)
		return MAX_SIGNAL_STR;
	if(algo.compare("MGMMean1")==0)
		return AVG_SIGNAL_STR;
	if(algo.compare("BeRebid1")==0)
		return MAX_SIGNAL_STR;
	if(algo.compare("MPGM2")==0)
		return MAX_SIGNAL_STR;
	if(algo.compare("MGMMean2")==0)
		return AVG_SIGNAL_STR;
	if(algo.compare("MPGM2")==0)
		return MAX_SIGNAL_STR;
	if(algo.compare("BeRebid2")==0)
		return MAX_SIGNAL_STR;
	if(algo.compare("SEI1")==0)
		return I_PARAM;
	if(algo.compare("SEI2")==0)
		return I_PARAM;
	//else error
	cout<<"Algorithm: "<<algo<<" could not be supported for moving."<<endl;
	exit(0);
}

/**
 * Berebid2 specific gain function
 *
 * @param neighborVals The value of neighbor links
 * @param numNeighbors The number of neighbors
 * @return The gain
 */
int move(string algo) {
	cout<<"MOVE_CODE_EXEC"<<endl;
	if(algo.compare("MPGM1")==0)
		return move_MPGM1();
	if(algo.compare("MGMMean1")==0)
		return move_MGMMean1();
	if(algo.compare("BeRebid1")==0)
		return move_BeRebid1();
	if(algo.compare("MPGM2")==0)
		return move_MPGM2();
	if(algo.compare("MGMMean2")==0)
		return move_MGMMean2();
	if(algo.compare("BeRebid2")==0)
		return move_BeRebid2();
	if(algo.compare("SEI1")==0)
		return move_SEI1();
	if(algo.compare("SEI2")==0)
		return move_SEI2();
	//else error
	cout<<"Algorithm: "<<algo<<" could not be supported for moving."<<endl;
	exit(0);
}

/**
 * Berebid2 specific move function
 */
int move_BeRebid2() {
	return standardForwardMovement();
}

/**
 * MPGM2 specific move function
 */
int move_MPGM2() {
	return standardForwardMovement();
}

/**
 * MGMMean2 specific move function
 */
int move_MGMMean2() {
	return standardForwardMovement();
}

/**
 * SEI2 specific move function
 */
int move_SEI2() {
	return standardForwardMovement();
}

/**
 * SEI1 specific move function
 */
int move_SEI1() {
	return standardForwardMovement();
}

/**
 * Sampling mode specific move function
 */
int move_sample() {
	standardForwardMovement();
	myLoc=0;
	return myLoc;
}

/**
 * MPGM1 specific move function
 */
int move_MPGM1() {
	return standardForwardMovement();
}

/**
 * MGMMean1 specific move function
 */
int move_MGMMean1() {
	return standardForwardMovement();
}

int standardForwardMovement() {
	myLoc = myLoc+1;
	move_forward(MOVEMENT_WAVE_LENGTH);
	return myLoc;
}

/**
 * Berebid1 specific move function
 */
int move_BeRebid1() {

	//get current round from matrix
	Round*  round = &rounds[numRoundsElapsed];

	if(round->ifIWinMyLoc==myLoc) {
		return myLoc;
	}
	else if(round->ifIWinMyLoc > myLoc) {
		myLoc = round->ifIWinMyLoc;
		return standardForwardMovement();
	}
	else {

		//compute how many steps to move
		int numToBackTrack = myLoc - round->ifIWinMyLoc;

		//turn around
		turn(180);
		
		for(int z=0; z < numToBackTrack; z++) {

			//backtrack movements
			standardForwardMovement();
		}

		//turn around
		turn(179);
		
		//update and return loc
		myLoc = round->ifIWinMyLoc;
		return myLoc;
	}
}

/**
 * Generates a histogram of samples.
 *
 * @param samples The samples array
 * @param numSamples The number of samples
 */
map<int,int>* genHistogram(int*  samples, int numSamples) {
	map<int, int>* rtn = new map<int,int>();

	for(int x=0; x < numSamples; x++) {
		int sample = samples[x];

		map<int,int>::iterator findResult = rtn->find(sample);
		if(findResult==rtn->end()) {
			//not found, put 1
			rtn->insert(pair<int,int>(sample, 1));
		}
		else {
			//else update freq
			findResult->second = findResult->second+1;
		}
	}

	return rtn;
}

/**
 * Computes the mean of an array of samples.
 */
double computeMean(int* samples, int numSamples) {
	double sum=0.0;
	for(int x=0; x < numSamples; x++) {
		sum+= samples[x];
	}

	return sum/numSamples;
}

/**
 * Computes the standard deviation of an array of samples.
 */
double computeStdDev(int* samples, int numSamples) {

	double mean = computeMean(samples, numSamples);

	double sumOfComponents=0.0;
	for(int x=0; x < numSamples; x++) {
		sumOfComponents+= pow(samples[x] - mean, 2);
	}

	return sqrt(sumOfComponents/numSamples);
}

/**
 * Computes term 3 of BeRebid equation. (see paper)
 */
double computeTerm3(int ts, double rb) {
	return ts*rb*compute_F(rb);
}

/**
 * Computes term 2 of BeRebid equation (see paper)
 */
double computeTerm2(int* samples, int numSamples, double rb, int te) {
	double rtn=0.0;
	for(int i=0; i < numSamples; i++) {
		if(samples[i] > rb)
			rtn+= samples[i] * compute_P(samples[i],te);
	}

	return rtn;
}

/**
 * Computes Gaussian
 *
 * @param x The value
 * @param u The mean
 * @param o stddev
 */
double computeGaussian(double x, double u, double o) {
	double expTerm = (-1.0) * (pow(x-u, 2.0)/(2.0*pow(o,2.0)));
	return 1.0/(o*sqrt(2.0*3.14))* exp(expTerm);
}

/**
 * Computes P function in BeRebid (see paper)
 */
double compute_P(double x, double te) {
	return te * compute_f(x) * pow(compute_F(x), te-1);
}

/**
 * Computes f function in BeRebid (see paper)
 */
double compute_f(double x) {
	return computeGaussian(x,mean, stddev);
}

/**
 * Computes F function in BeRebid (see paper)
 */
double compute_F(double x) {
	double errorTerm  = (x - mean) / (stddev * sqrt(2.0));
	return 1.0/2.0 * (1.0 + erf(errorTerm));
}