#ifndef _DEVICE_
#define _DEVICE_

#include "device.h"


	/* PUBLIC */

		device::device(fileHandler device_file){

			if(device_file.isFileValid()){

				std::multimap<std::string, std::string> device_contents = device_file.getParams();

				for(std::multimap<std::string,std::string>::iterator it = device_contents.begin(); it != device_contents.end(); ++it){

					std::string next[] = {it->first, it->second};

					if(next[0].substr(0, 6) == "family"){ // read in device family name

						name_ = next[1] + ", " + name_;

					}else if(next[0].substr(0, 5) == "model"){ // read in device model name

						name_ = name_ + next[1];

					}

				}

			}else std::cout << "\n\nError: BAD DEVICE FILE!\n\n";
		}


		//void parseModules(std::string mem_file);

		//void simulateApplication(std::string trace_file);


	/* PRIVATE */
		//storageUnit& associativityToRegion(unsigned module_index);

		//storageUnit& findInCache(unsigned module_index);

		//std::vector<storageUnit> pr_regions_;

		//unsigned char prr_associativity_;

		//std::vector<storageUnit> cache_memory_;

		//std::vector<replAlg> repl_algs_;

#endif
