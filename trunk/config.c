#include <vector>
#include "config.h"
#include "Network.h"

//load configuration
void loadConfiguration() {

//init maps
robotIDToIP = new map<int, string>();
hwAddrToID = new map<string, int>();
neighborMap = new map<int, vector<int>*>();

	//load ip map
	robotIDToIP->insert(std::pair<int, string>(0, "127.0.0.1"));
	robotIDToIP->insert(std::pair<int, string>(1, "127.0.0.1"));
	
	//load hardware addr map
	hwAddrToID->insert(std::pair<string,int>("57:57:57:57:6A:42", 0));
	hwAddrToID->insert(std::pair<string,int>("57:57:57:57:6A:44", 1));
	
	//init connections map -- robot 0
	vector<int>* neighbors0 = new vector<int>();
	neighbors0->push_back(1);
	neighborMap->insert(std::pair<int, vector<int>*>(0, neighbors0));
	
	//init connections map -- robot 1
	vector<int>* neighbors1 = new vector<int>();
	neighbors1->push_back(0);
	neighborMap->insert(std::pair<int, vector<int>*>(1, neighbors1));
	
}
