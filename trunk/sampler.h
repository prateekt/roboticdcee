#ifndef SAMPLER_H
#define SAMPLER_H
#include "Network.h"

/**
 -----
 * The sampler controls the signal strength sampling routine for Be-Rebid. Users can specify
 * the number of movements the robots do, the number of samples per movement,
 * and then number of neighbors involved in the sampling.
 -----
 */

void getSamples(int numSamples,int,int);
int genSample(int);

extern int* signalStrSamples;
extern int numSignalStrSamples;
extern int mean;
extern int stddev;

#endif
