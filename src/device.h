#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "fileHandler.h"
#include "storageUnit.h"
//#include "algs/randomAlg.h"


class device{

	public:
		device(fileHandler device_file);


		void parseModules(storageUnit memory_hierarchy);

		void simulateApplication(std::vector<std::string> trace_file, unsigned long long int stop_ccc = -1);

		std::string name_, file_;

	private:
		storageUnit& associativityToRegion(unsigned module_index);

		storageUnit& findInCache(unsigned module_index);

		std::vector<storageUnit> pr_regions_;

		unsigned char prr_associativity_;

		std::vector<storageUnit*> memory_hierarchy_;
		
		//std::vector<replAlg*> repl_algs_;
};

#endif
