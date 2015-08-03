#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <queue>

#include "fileHandler.h"
#include "storageUnit.h"
//#include "algs/randomAlg.h"


class device{

	enum memAction { SEARCH, TRANSFER, EXECUTE, VACANT, IDLE, UNKNOWN };

	public:
		device(fileHandler device_file);


		void parseModules(storageUnit memory_hierarchy);

		void simulateApplication(std::vector<std::string> trace_file, unsigned long long int stop_ccc = -1);

		std::string name_, file_;

	private:
		storageUnit& associativityToRegion(unsigned module_index);

		storageUnit& findInCache(unsigned module_index);

		std::tuple<unsigned, unsigned, unsigned> parseTrace(std::string trace);

		std::vector<storageUnit> pr_regions_;

		unsigned char prr_associativity_;

		std::vector<storageUnit*> memory_hierarchy_;
		
		storageUnit* memory_hierarchy_top_;

};

#endif
