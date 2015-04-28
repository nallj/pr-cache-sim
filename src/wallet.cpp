#ifndef _WALLET_
#define _WALLET_

#include "wallet.h"


/* PUBLIC */

	// wallet acts as a library for drachma
	wallet::wallet(){

		std::vector<std::string> wallet_params {"name", "d", "m", "t"};
		reader wallet_reader = reader("wallet.wal", wallet_params);

		if(wallet_reader.isFileValid()){

			std::multimap<std::string, std::string> wallet_contents = wallet_reader.getParams();

			/*std::cout << "> WALLET MULTIMAP <\n";
			for(std::multimap<std::string,std::string>::iterator it = wallet_contents.begin(); it != wallet_contents.end(); ++it)
				std::cout << "MAP: " << it->first << "  \t=>\t" << it->second << std::endl;*/

			for(std::multimap<std::string,std::string>::iterator it = wallet_contents.begin(); it != wallet_contents.end(); ++it){

				std::string next[] = {it->first, it->second};

				if(next[0].substr(0, 1) == "d"){ // read in device file

					device new_dev = device(next[1]);
					devices_.push_back(new_dev);

				}else if(next[0].substr(0, 1) == "m"){ // read in memory file

					storageUnit new_mem = buildMemoryHierarchy(next[1]);
					memories_.push_back(new_mem);

				}else if(next[0].substr(0, 1) == "t"){ // trace file list

					trace_files_.push_back(next[1]);
				}

			}

			// wallet initialization complete

		} else std::cout << "\n\nWALLET FILE IS CORRUPT!!!\n\n";
	}


	void wallet::printDetails(){

		std::cout << "The wallet contains " << devices_.size() << " devices, "
				  << memories_.size() << " memories, and " << trace_files_.size() << " trace files.\n";

		std::cout << "\nDEVICE LIST:\n";
		for(int i = 0; i < devices_.size(); i++)
			std::cout << "\t* '" << devices_[i].name_ << "'\n";

		std::cout << "\nMEMORY LIST:\n";
		for(int i = 0; i < memories_.size(); i++){

			std::cout << "   <Start Configuration>\n";

			storageUnit* ptr = &(memories_[i]);
			while(ptr){
				ptr->printDetails();
				ptr = ptr->getChild();
			}

			std::cout << "   <End Configuration>\n";
		}

		std::cout << "\nTRACE FILE LIST:\n";
			for(int i = 0; i < trace_files_.size(); i++)
				std::cout << "\t* " << trace_files_[i] << "\n";

		std::cout << std::endl << std::endl;
	}


/* PRIVATE */

	storageUnit wallet::buildMemoryHierarchy(std::string memory_file){

		std::vector<std::string> memory_params {"name", "main name", "main size", "main read latency", "main search latency"};
		std::vector<std::string> regex_filter {"[l][1-9] \\bname\\b[:] [\\w ]+",
												"[l][1-9] \\bsize\\b[:] [\\w ]+",
												"[l][1-9] \\b(read|search)\\b \\blatency\\b[:] [\\w ]+"};


		reader memory_reader = reader("memory.mem", memory_params, regex_filter);

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

			storageUnit main_memory = storageUnit(std::stoi(main_mem_size), std::stoi(main_mem_read), std::stoi(main_mem_search));
			main_memory.name_ = main_mem_name;


			// create levels (up to L9) of cache based on data parameters
			std::vector<std::string> level_params = memory_reader.getData();
			std::string level_name[10];
			int level_size[10], level_read[10], level_search[10], lowest_level = 0;

			for(int i = 0; i < level_params.size(); i++){

				int which_level = atoi(level_params[i].substr(1, 1).c_str()); // grab level number
				std::string param = level_params[i].substr(3, level_params[i].find(":") - 3); // grab rest of param
				std::string arg = level_params[i].substr(level_params[i].find(":") + 2); // grab param value

				if(lowest_level < which_level) // keep track of the lowest level declared
					lowest_level = which_level;

				if(param == "name") // read in level's name
					level_name[which_level] = arg + " [L" + std::to_string(which_level) + "]";

				else if(param == "size") // read in level's size
					level_size[which_level] = std::stoi(arg);

				else if(param == "read latency") // read in level's read latency (in CCs)
					level_read[which_level] = std::stoi(arg);

				else if(param == "search latency") // read in level's search latency (in CCs)
					level_search[which_level] = std::stoi(arg);
			}


			// if cache levels have been declared, link them all together from the main memory to the lowest level
			if(lowest_level > 0){

				storageUnit* parent = &main_memory;

				for(int i = 1; i <= lowest_level; i++){

					storageUnit* child = new storageUnit(level_size[i], level_read[i], level_search[i]);
					child->name_ = level_name[i];

					parent->setChild(child);
					parent = child;
				}
			}

			return main_memory;

		}else std::cout << "\n\nMEMORY FILE IS CORRUPT!!!\n\n";
	}

#endif
