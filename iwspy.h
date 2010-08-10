#ifndef IWSPY_H
#define IWSPY_H

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
 -----
 * The IWSPY API.
 -----
 */

//function prototypes
vector<string>* getFile(string file);
map<string, int>* getMap(vector<string>* lines);
vector<string>* exec(string cmd);
map<string, int>* getIWSPYResults();
bool regWithIWSPY(string interface, string IP);
void TrimSpaces(string& str);
bool regAllWithIWSPY();
map<int,int>* getIWSPYResultsByAgent();
#endif
