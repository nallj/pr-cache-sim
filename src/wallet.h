#ifndef _WALLET_H_
#define _WALLET_H_

#include <vector>
#include <string>
#include <sstream>
//#include <utility>
#include <iostream>
//#include <fstream>
//#include <cstring>
//#include <stdlib.h>

#include "reader.h"
#include "device.h"
#include "storageUnit.h"


class wallet{

	public:
		wallet();


		void printDetails();

		void addDevice();

		void removeDevice();

		std::string name_;


	private:
		storageUnit buildMemoryHierarchy(std::string memory_file);

		std::vector<device> devices_;

		std::vector<storageUnit> memories_;

		//std::vector< std::vector<std::string> > traces_;
		std::vector<std::string> trace_files_;
};

#endif
