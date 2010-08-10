#include "sampler.h"
#include "dcop.h"
#include "create_lib.h"

using namespace std;

/**
 -----
 * The sampler controls the signal strength sampling routine for Be-Rebid. Users can specify
 * the number of movements the robots do, the number of samples per movement,
 * and then number of neighbors involved in the sampling.
 -----
 */

/**
 * The array of samples.
 */
int* signalStrSamples;

/**
 * The size of the samples array.
 */
int numSignalStrSamples;

/**
 * The computed mean of the samples.
 */
int mean;

/**
 * The computed standard deviation of the samples.
 */
int stddev;

/**
 * Executes the sampling routine. Takes samples of
 * sum of signal strengths with neighbors.
 *
 * @param numSamples The overall number of samples to take
 * @param numMovements The number of movements to make in taking those samples.
 * @param numNeighbors The number of neighbors involved with sampling.
 */
void getSamples(int numSamples, int numMovements, int numNeighbors) {
	signalStrSamples = new int[numSamples];

	int samplesTaken = 0;

	//for a certain of movements
	for(int x=0; x  < numMovements; x++) {
		move_sample();
		cout<<"Movement: "<<x<<endl;

		//for a certain number of samples per movement.
		for(int y=0; y < numSamples/numMovements; y++) {
			usleep(500);

			int sample = 0;

			//take legitimate samples - a sample of 0 does not count.
			while(sample==0) {
				sample = genSample(numNeighbors);
				if(sample!=0) {
					signalStrSamples[samplesTaken] = sample;
					cout<<"SAMPLE "<<samplesTaken<<": "<<signalStrSamples[samplesTaken]<<endl;
					samplesTaken++;
				}
				else {
					usleep(500);
					cout<<"Backoff"<<endl;
				}
			}
		}
	}
	cout<<"Exiting"<<endl;

	numSignalStrSamples= samplesTaken;

	//compute statistics on the samples
	mean = computeMean(signalStrSamples, numSignalStrSamples);
	stddev = computeStdDev(signalStrSamples, numSignalStrSamples);
	cout<<"MEAN: "<<mean<<" STDDEV: "<<stddev<<endl;
}

/**
 * Generates a single sample based on the neighborhood configuration. A
 * sample is the sum of the signal strengths the robot has with its
 * neighbors.
 *
 * @param numNeighbors The number of neighbors
 * @return The sum of signal strengths wit those neighbors.
 */
int genSample(int numNeighbors) {
	int* neighborVals = new int[numNeighbors+1];

	//clear array
	for(int x=0; x < NUM_AGENTS; x++) {
		neighborVals[x]=0;
	}

	//populate array with signal strengths
	updateSignalStrengths(neighborVals, numNeighbors);

	//sum signal strengths
	int sum=0;
	for(int x=0; x < getNeighbors(myRobotID)->size(); x++) {
		int neighbor = getNeighbors(myRobotID)->at(x);
		sum+= neighborVals[neighbor];
	}

	//return the sample
	return sum;
}