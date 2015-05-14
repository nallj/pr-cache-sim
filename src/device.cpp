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


		void device::parseModules(storageUnit memory_hierarchy){

			// NEXT 5 LINE IS THE OFFENDING CODE
			storageUnit* ptr = new storageUnit(memory_hierarchy);
			memory_hierarchy_.push_back(ptr);

			while( ptr = ptr->getChild() )
				memory_hierarchy_.push_back(ptr);


			/*storageUnit* ptr = &memory_hierarchy;
			memory_hierarchy_.push_back(ptr);

			while( ptr = ptr->getChild() )
			memory_hierarchy_.push_back(ptr);*/


			/*for(int i = 0; i < memory_hierarchy_.size(); i++)
				memory_hierarchy_[i]->printDetails();*/

			/*std::cout << "\n";
			module* lol = memory_hierarchy_[0]->getModule(0);
			std::cout << "dev.parseModule ID: " << lol->getId() << " with " << lol->getSize() << std::endl;

			lol = memory_hierarchy_[0]->getModule(1);
			std::cout << "dev.parseModule ID: " << lol->getId() << " with " << lol->getSize() << std::endl;

			lol = memory_hierarchy_[0]->getModule(2);
			std::cout << "dev.parseModule ID: " << lol->getId() << " with " << lol->getSize() << std::endl;

			lol = memory_hierarchy_[0]->getModule(15);
			std::cout << "dev.parseModule ID: " << lol->getId() << " with " << lol->getSize() << std::endl;*/

			//lol = memory_hierarchy_[0]->getModule(111);
			//std::cout << "ID: " << lol->getId() << " with " << lol->getSize() << std::endl;


			/*for(unsigned short i = 0; i < memory_hierarchy_.size(); i++){

				for(unsigned short j = 0; j < memory_hierarchy_[i]->getSize(); j++){

				}

			}*/

		}

		void device::simulateApplication(std::vector<std::string> trace_file, unsigned long long int stop_ccc){

			unsigned long long int ccc = 0; // clock cycle count
			unsigned long long int max_ccc = -1;

			unsigned latency_counters[memory_hierarchy_.size()];
			bool isSearching[memory_hierarchy_.size()]; // false implies reading


			for(; ccc < trace_file.size(); ccc++){

				std::cout << "\n<TRACE " << ccc << ">\n";

				//std::string next_addr = trace_file[ccc];
				unsigned next_addr = stoi( trace_file[ccc] ); // no validation?

				//signed short whos_busy = -1;
				// search for first busy level


				// FOR NOW THE ONLY CONCERN IS TO TEST AN L1
				// find a place for the new module
				memory_hierarchy_[1]->attemptModule(next_addr);







				// DEBUG: solid module index check; test for Ln's contents
				if(ccc == stop_ccc || ccc == (trace_file.size() - 1)){ // only test up to "stop_ccc" traces or the end of trace
					std::cout << "\n";

					for(unsigned short n = memory_hierarchy_.size() - 1; 0 < n; n--){
						std::cout << "\n\t>>> Contents of L" << n << " <<<\n";

						for(int i = 0; i < memory_hierarchy_[0]->getSize(); i++){
							if(memory_hierarchy_[n]->isModulePresent( i ))
								std::cout << "module " << i << " is PRESENT @ L" << n << "\n";
							else
								std::cout << "module " << i << " is not present @ L" << n << "\n";
						}
					}

					break;
				}

				// safety stop
				//if(ccc == 101) break;


				if(ccc == max_ccc){
					std::cout << "Warning: Reached Drachma's present operational limits. Simulation ceased.";
					//break;
				}
			}
		}


	/* PRIVATE */

		//storageUnit& device::associativityToRegion(unsigned module_index);

		//storageUnit& device::findInCache(unsigned module_index);

		//std::vector<storageUnit> pr_regions_;

		//unsigned char prr_associativity_;

		std::vector<storageUnit*> memory_hierarchy_;

		//std::vector<replAlg> repl_algs_;

#endif
