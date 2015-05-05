#ifndef _WALLET_H_
#define _WALLET_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "fileHandler.h"
#include "device.h"
#include "storageUnit.h"


// wallet acts as a library for drachma
class wallet{

	public:
		wallet();


		void printDetails();

		void addDevice(std::string file_name);

		bool removeDevice(std::string file_name);

		void addMemory(std::string file_name);

		bool removeMemory(std::string file_name);

		void addTraceFile(std::string file_name);

		bool removeTraceFile(std::string file_name);

		std::string name_;


	private:
		fileHandler wallet_handler_;

		storageUnit buildMemoryHierarchy(std::string memory_file);

		std::vector<device> devices_;

		std::vector<storageUnit> memories_;

		std::vector<std::string> trace_files_;
};


#endif
