#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "iwspy.h"
#include "Network.h"
#include "dcop.h"

using namespace std;

/**
 -----
 * The IWSPY API.
 -----
 */

/**
 * Registers all neighbors with this robot's IWSPY utility.
 * After calling this function, polling IWSPY will return
 * the signal strenghs of the neighbor links.
 */
bool regAllWithIWSPY() {
	vector<int>* neighbors = getNeighbors(myRobotID);
	for(int x=0; x < neighbors->size(); x++) {
		int neighbor = neighbors->at(x);
		bool worked = regWithIWSPY("ath0", getIP(neighbor));
		if(!worked) {
			cout<<"Failed to IWSPY: "<<getIP(neighbor)<<endl;
			return false;
		}
	}
	return true;
}

/**
 * Registers a single IP with IWSPY
 *
 * @param interface The interface to use (use "ath0" for our configuration)
 * @param IP The IP address
 * @return Whether registration was successful or not
 */
/*bool regWithIWSPY(string interface, string IP) {
	string cmd = "sudo iwspy " + interface + " + " + IP;
	vector<string>* result = exec(cmd);
	return result->size() == 0;
}*/

bool regWithIWSPY(string interface, string IP) {
	cout<<"FAKE REG: "<<interface<<IP<<endl;
	return true;
}

map<int,int>* getIWSPYResultsByAgent() {
	map<int, int>* rtn = new map<int,int>();
	for(int x=0; x < NUM_AGENTS; x++) {
		if(x!=myRobotID) {
			int result = rand()%70+1;
			cout<<"INSERT PAIR: "<<result<<endl;
			rtn->insert(std::pair<int,int>(x,(result)));
		}
	}
	return rtn;
}

/**
 * Returns IWSPY results in [agent, RSSI] mapping.
 */
/*map<int, int>* getIWSPYResultsByAgent() {
	map<string, int>* r = getIWSPYResults();
	map<int,int>* rtn = new map<int,int>();
	cout<<"IWSPY MAP: {";
	for(map<string,int>::iterator i1=r->begin(); i1!=r->end(); ++i1) {
		int id = getID((*i1).first);
		int ss = (*i1).second;
		rtn->insert(std::pair<int,int>(id,ss));
		cout<<"{"<<id<<","<<ss<<"}";
	}
	cout<<"}"<<endl;
	return rtn;
}*/

/**
 * Returns IWSPY Results in line form. Helper function to other functions.
 */
map<string, int>* getIWSPYResults() {
	vector<string>* lines = exec("iwspy ath0");
	return getMap(lines);
}

/**
 * String Trim Function
 */
void TrimSpaces(string& str) {
	size_t startpos = str.find_first_not_of(" \t");
	size_t endpos = str.find_last_not_of(" t");

	if((string::npos == startpos) || (string::npos == endpos)) {
		str = "";
	}
	else
		str = str.substr(startpos, endpos-startpos+1);
}

/**
 * Executes a command on unix command prompt.
 *
 * @param cmd The command to execute
 * @return The trace from running the command as a series of lines.
 */
vector<string>* exec(string cmd) {
	vector<string>* rtn = new vector<string>();

	FILE* pipe = popen(cmd.c_str(), "r");
	if(!pipe)return NULL;
	char buffer[128];
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL) {
			rtn->push_back(buffer);
			//cout<<buffer<<endl;
		}
	}
	pclose(pipe);
	return rtn;
}

/*int main() {
	vector<string>* lines = getFile("test");
	vector<string>* result = exec("iwspy");
	map<string, int>* it = getMap(lines);

	for(map<string, int>::iterator i1 = it->begin(); i1!=it->end(); ++i1)
	{
		cout<<(*i1).first<<" === "<<(*i1).second<<endl;
	}

}*/

/**
 * Parses IWSPY trace. Returns a mapping of [hw address, link quality to that hardware]
 *
 * @param lines The IWSPY trace
 * @return The map described
 */
map<string, int>* getMap(vector<string>* lines) {

	map<string, int>* rtn = new map<string, int>();

	for(vector<string>::const_iterator it = lines->begin(); it!=lines->end(); ++it) {

		string s = *it;

		if(s.find("Quality",0)!= string::npos && s.find("Signal level",0)!=string::npos && s.find("Noise level",0)!=string::npos && s.find("Link",0)==string::npos && s.find("Typical")==string::npos) {

			//isolate hw
			string hw = s.substr(0,s.find(" : ",0));
			TrimSpaces(hw);

			//isolate signal str
			string signalLevelStr = s.substr(s.find("Quality")+8);
			signalLevelStr = signalLevelStr.substr(0, signalLevelStr.find("/"));
			//std::remove(signalLevelStr.begin(), signalLevelStr.end(), ' ');
			int val = atoi(signalLevelStr.c_str());

			//put in map
			rtn->insert(std::pair<string, int>(hw, val));
		}

	}
	return rtn;
}

/**
 * Returns file content as a vector of lines.
 */
vector<string>* getFile(string file) {

	vector<string>* rtn = new vector<string>();

	char buffer[256];
	ifstream myfile(file.c_str());
	while(!myfile.eof()) {
		myfile.getline(buffer,1000);
		string* str = new string(buffer);
		rtn->push_back(*str);
	}

	return rtn;
}

