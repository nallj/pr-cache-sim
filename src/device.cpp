#ifndef _DEVICE_
#define _DEVICE_

#include "device.h"


	/* PUBLIC */

		device::device(fileHandler device_file) : file_(device_file.input_file_){

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

			storageUnit* ptr = new storageUnit(memory_hierarchy);
			memory_hierarchy_.push_back(ptr);

			std::vector<storageUnit*> reversal_list;
			while( ptr = ptr->getChild() )
				reversal_list.push_back(ptr);

			for(int i = reversal_list.size() - 1; i >= 0; i--)
				memory_hierarchy_.push_back( reversal_list[i] );


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
			unsigned traces_processed = 0;

			// arrays are used instead of single variables in preparation for
			// asynchronous level processing in future Drachma iterations
			unsigned short latency_counters[memory_hierarchy_.size()];
			bool isSearching[memory_hierarchy_.size()]; // false implies reading

			unsigned next_addr = 0;
			unsigned short whos_busy = 1; // always begin with L1
			std::string busy_slug; // short, human-friendly name of who is busy
			bool addr_resolved = true;


			// main simulation loop
			for(; traces_processed < trace_file.size(); ccc++){

				if(whos_busy > 0) busy_slug = "L" + std::to_string(whos_busy);
				else busy_slug = "Main Memory";

				std::cout << "\n<CC Count: " << ccc << " / Traces Accessed: " << traces_processed << " / ASKING: " << busy_slug << " for " << std::stoi( trace_file[traces_processed] ) << ">\n";

				// todo: make sure each memory level is busy (if possible); asynchronous search and reads?

				// initiate search cycle on level "whos_busy"
				if(memory_hierarchy_[whos_busy]->availableToRead()){

					if(addr_resolved){
						addr_resolved = false;
						next_addr = std::stoi( trace_file[traces_processed] );
					}

					memory_hierarchy_[whos_busy]->toggleReadLock();

					latency_counters[whos_busy] = memory_hierarchy_[whos_busy]->getSearchLatency();
					std::cout << busy_slug << " is busy SEARCHING [" << --latency_counters[whos_busy] << " more cc]; must WAIT\n";

					isSearching[whos_busy] = true;

				// continue interaction with specific memory level
				}else{

					// busy searching memory level
					if(isSearching[whos_busy]){

						// search cycle
						if(--latency_counters[whos_busy] > 0)
							std::cout << busy_slug << " is busy SEARCHING [" << latency_counters[whos_busy] << " more cc]; must WAIT\n";

						// search completed
						else{
							std::cout << busy_slug << " is done SEARCHING";

							// if memory module present
							if(memory_hierarchy_[whos_busy]->isModulePresent(next_addr)){
								std::cout << "  -  the module is at this level, begin READING\n";

								isSearching[whos_busy] = false; // begin reading phase
								latency_counters[whos_busy] = memory_hierarchy_[whos_busy]->getReadLatency();

							// module was not found at this level
							}else{
								std::cout << "  -  module not present, SEARCH next level\n";

								memory_hierarchy_[whos_busy]->toggleReadLock();

								if(whos_busy == memory_hierarchy_.size() - 1)
									whos_busy = 0; // not found in cache, request from main mem
								else
									whos_busy++; // search in next level of cache
							}
						}

					// or busy reading
					}else{

						// read cycle
						if(--latency_counters[whos_busy] > 0)
							std::cout << busy_slug << " is busy READING [" << latency_counters[whos_busy] << " more cc]; must WAIT\n";

						else{
							std::cout << busy_slug << " is done READING\n";
								// cache hit is achieved

							if(whos_busy != 1) memory_hierarchy_[1]->attemptModule(next_addr);
								// update lowest level of cache if not in lowest cache
							memory_hierarchy_[whos_busy]->toggleReadLock();

							whos_busy = 1;
							addr_resolved = true;
							traces_processed++;
						}
					}

				}





				// DEBUG: MULTI COLUMNLAR - solid module index check; test for Li's contents
				if(ccc == stop_ccc || traces_processed == (trace_file.size() - 1)){ // only test up to "stop_ccc" traces or the end of trace
					std::cout << "\n\n";

					for(int i = 0; i < memory_hierarchy_[0]->getSize(); i++){

						if(i == 0){
							for(unsigned short j = memory_hierarchy_.size() - 1; 0 < j; j--){
								std::cout << "\t>>> Contents of L" << j << " <<<";

								if(j != 1) std::cout << "\t\t\t";
							}

							std::cout << "\n";
						}

						for(unsigned short j = memory_hierarchy_.size() - 1; 0 < j; j--){

							if(memory_hierarchy_[j]->isModulePresent( i ))
								std::cout << "module " << i << " is PRESENT @ L" << j;
							else
								std::cout << "module " << i << " is not present @ L" << j;

							if(j == 1) std::cout << "\n";
							else  std::cout << "\t\t\t";
						}

					}

					break;
				}


				// DEBUG: SINGLE COLUNAR - solid module index check; test for Li's contents
				/*if(ccc == stop_ccc || traces_processed == (trace_file.size() - 1)){ // only test up to "stop_ccc" traces or the end of trace
					std::cout << "\n";

					for(unsigned short i = memory_hierarchy_.size() - 1; 0 < i; i--){
						std::cout << "\n\t>>> Contents of L" << i << " <<<\n";

						for(int j = 0; j < memory_hierarchy_[0]->getSize(); j++){
							if(memory_hierarchy_[i]->isModulePresent( j ))
								std::cout << "module " << j << " is PRESENT @ L" << i << "\n";
							else
								std::cout << "module " << j << " is not present @ L" << i << "\n";
						}
					}

					break;
				}*/

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
