#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <string>
#include <vector>

#include "reader.h"
#include "storageUnit.h"
#include "replAlg.h"


class device{

	public:
		device(std::string device_file);


		void parseModules(std::string mem_file);

		void simulateApplication(std::string trace_file);

		std::string name_;

	private:
		storageUnit& associativityToRegion(unsigned module_index);

		storageUnit& findInCache(unsigned module_index);

		std::vector<storageUnit> pr_regions_;

		unsigned char prr_associativity_;

		std::vector<storageUnit> cache_memory_;
		
		std::vector<replAlg> repl_algs_;
};

#endif
