#ifndef _WALLET_H_
#define _WALLET_H_

//#include <string>
#include <vector>
//#include <utility>
#include <iostream>
#include <fstream>
//#include <cstring>
#include <stdlib.h>

#include "reader.h"
#include "device.h"
#include "storageUnit.h"

class wallet{

	public:
		wallet();

		void addDevice();

		void removeDevice();

	private:
		std::map<std::string, std::string> param_dictionary;

		std::vector<device> devices;

		std::vector<storageUnit> memories;

		std::vector< std::vector<std::string> > traces;
};

#endif
