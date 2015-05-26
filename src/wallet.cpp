#ifndef _WALLET_
#define _WALLET_

#include "wallet.h"


/* PUBLIC */

	wallet::wallet(){

		std::vector<std::string> wallet_params {"name", "d", "m", "t"};
		wallet_handler_ = fileHandler("wallet.wal", wallet_params);

		if(wallet_handler_.isFileValid()){

			std::multimap<std::string, std::string> wallet_contents = wallet_handler_.getParams();

			/*std::cout << "> WALLET MULTIMAP <\n";
			for(std::multimap<std::string,std::string>::iterator it = wallet_contents.begin(); it != wallet_contents.end(); ++it)
				std::cout << "MAP: " << it->first << "  \t=>\t" << it->second << std::endl;*/

			for(std::multimap<std::string,std::string>::iterator it = wallet_contents.begin(); it != wallet_contents.end(); ++it){

				std::string next[] = {it->first, it->second};

				if(next[0].substr(0, 1) == "d"){ // read in device file

					std::vector<std::string> dev_params {"family", "model", "slices"};
					fileHandler dev_reader = fileHandler(next[1], dev_params);

					device new_dev = device(dev_reader);
					devices_.push_back(new_dev);

				}else if(next[0].substr(0, 1) == "m"){ // read in memory file

					storageUnit new_mem = buildMemoryHierarchy(next[1]);
					memories_.push_back(new_mem);

				}else if(next[0].substr(0, 1) == "t"){ // trace file list

					trace_files_.push_back(next[1]);
				}

			}

		} else std::cout << "\n\nError: BAD WALLET FILE!\n\n";
	}


	void wallet::printDetails(){

		std::cout << "The wallet contains " << devices_.size() << " devices, "
				  << memories_.size() << " memories, and " << trace_files_.size() << " trace files.\n";

		std::cout << "\n\t== DEVICE LIST (with ID): == \n";
		for(int i = 0; i < devices_.size(); i++)
			std::cout << "\t\t" << i << ") '" << devices_[i].name_ << "' <" << devices_[i].file_ << ">\n";

		std::cout << "\n\n\t== MEMORY LIST (with ID): ==\n";
		for(int i = 0; i < memories_.size(); i++){

			std::cout << "\t\t" << i << ") '" << memories_[i].name_ << "' <" << memories_[i].file_ << ">\n";

			storageUnit* ptr = &(memories_[i]);
			while(ptr){
				ptr->printDetails();
				ptr = ptr->getChild();
			}

			std::cout << "\n";
		}

		std::cout << "\n\t== TRACE FILE LIST (with ID): ==\n";
			for(int i = 0; i < trace_files_.size(); i++)
				std::cout << "\t\t" << i << ") " << trace_files_[i] << "\n";

		std::cout << std::endl << std::endl;
	}

	void wallet::addDevice(std::string file_name){
		wallet_handler_.addToFile("d: " + file_name);
	}

	bool wallet::removeDevice(std::string file_name){
		return wallet_handler_.removeFromFile("d: " + file_name);
	}

	void wallet::addMemory(std::string file_name){
		wallet_handler_.addToFile("m: " + file_name);
	}

	bool wallet::removeMemory(std::string file_name){
		return wallet_handler_.removeFromFile("m: " + file_name);
	}

	void wallet::addTraceFile(std::string file_name){
		wallet_handler_.addToFile("t: " + file_name);
	}

	bool wallet::removeTraceFile(std::string file_name){
		return wallet_handler_.removeFromFile("t: " + file_name);
	}


	device wallet::getDevice(unsigned device_id){
		return devices_[device_id];
	}

	storageUnit wallet::getMemory(unsigned memory_id){
		return memories_[memory_id];
	}

	std::vector<std::string> wallet::getTraceFile(unsigned trace_id){

		std::vector<std::string> trace_params {"name"};
		std::vector<std::string> regex_filter {"\\d+"};

		fileHandler trace_reader = fileHandler(trace_files_[trace_id], trace_params, regex_filter);

		return trace_reader.getData();
	}


/* PRIVATE */

	storageUnit wallet::buildMemoryHierarchy(std::string memory_file){

		std::vector<std::string> memory_params {"name", "main name", "main size", "main read latency", "main search latency"};
		std::vector<std::string> regex_filter {"[l][1-9] \\bname\\b[:] [\\w ]+",
												"[l][1-9] \\bsize\\b[:] [\\w ]+",
												"[l][1-9] \\b(read|search)\\b \\blatency\\b[:] [\\w ]+"};


		fileHandler memory_reader = fileHandler(memory_file, memory_params, regex_filter);

		if(memory_reader.isFileValid()){

			// start by retrieving the information needed to form the main menu
			std::multimap<std::string, std::string> memory_contents = memory_reader.getParams();
			std::string mem_name, main_mem_name, main_mem_size, main_mem_read, main_mem_search;

			for(std::multimap<std::string,std::string>::iterator it = memory_contents.begin(); it != memory_contents.end(); ++it){

				std::string next[] = {it->first, it->second};

				if(next[0] == "name") // read in memory configuration's "name"
					mem_name = next[1];

				else if(next[0] == "main name") // read in main memory name
					main_mem_name = next[1];

				else if(next[0] == "main size") // read in main memory size
					main_mem_size = next[1];

				else if(next[0] == "main read latency") // read in main memory read latency (in CCs)
					main_mem_read = next[1];

				else if(next[0] == "main search latency") // read in main memory search latency (in CCs)
					main_mem_search = next[1];
			}

			storageUnit main_memory = storageUnit(std::stoi(main_mem_size), NULL, std::stoi(main_mem_read), std::stoi(main_mem_search));
			//storageUnit main_memory = storageUnit(mem_name, memory_file, std::stoi(main_mem_size), NULL, std::stoi(main_mem_read), std::stoi(main_mem_search));
			if(mem_name != "") main_memory.name_ = main_mem_name;
			main_memory.file_ = memory_file;

			// initialize main memory's modules
			for(unsigned i = 0; i < std::stoi(main_mem_size); i++)
				main_memory.insertModule( new module(i, 777) );
					// todo: bitstream width is arbitrary since it is not yet considered in Drachma


			// create levels (up to L9) of cache based on data parameters
			std::vector<std::string> level_params = memory_reader.getData();
			std::string level_name[10];
			int level_size[10], level_read[10], level_search[10], highest_level = 0;

			for(int i = 0; i < level_params.size(); i++){

				int which_level = atoi(level_params[i].substr(1, 1).c_str()); // grab level number
				std::string param = level_params[i].substr(3, level_params[i].find(":") - 3); // grab rest of param
				std::string arg = level_params[i].substr(level_params[i].find(":") + 2); // grab param value

				if(highest_level < which_level) // keep track of the lowest level declared
					highest_level = which_level;

				if(param == "name") // read in level's name
					level_name[which_level] = arg + " [L" + std::to_string(which_level) + "]";

				else if(param == "size") // read in level's size
					level_size[which_level] = std::stoi(arg);

				else if(param == "read latency") // read in level's read latency (in CCs)
					level_read[which_level] = std::stoi(arg);

				else if(param == "search latency") // read in level's search latency (in CCs)
					level_search[which_level] = std::stoi(arg);
			}


			// if cache levels have been declared, link them all together from the lowest level to the main memory
			if(highest_level > 0){

				storageUnit* parent = &main_memory;

				for(int i = highest_level; i > 0; i--){

					// use only random replacement
					//replAlg* default_alg = new randomAlg("Random Replacement", level_size[i]);
					// use only random replacement
					replAlg* default_alg = new fifoAlg("FIFO Replacement", level_size[i]);

					storageUnit* child = new storageUnit(level_size[i], default_alg, level_read[i], level_search[i]);

					child->name_ = level_name[i];
					if(i != highest_level) child->setParent(parent); // dont make main memory the parent of Ln

					parent->setChild(child);
					parent = child;
				}
			}

			return main_memory;

		}else std::cout << "\n\nMEMORY FILE IS CORRUPT!!!\n\n";
	}


#endif
