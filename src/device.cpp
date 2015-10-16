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


		// todo: consider changing name to "associateHierarchy"
		void device::parseModules(storageUnit memory_hierarchy){

			/*storageUnit* ptr = new storageUnit(memory_hierarchy);
			memory_hierarchy_.push_back(ptr);

			std::vector<storageUnit*> reversal_list;
			while( ptr = ptr->getChild() )
				reversal_list.push_back(ptr);

			for(int i = reversal_list.size() - 1; i >= 0; i--)
				memory_hierarchy_.push_back( reversal_list[i] );*/

			// is this necessary?
			memory_hierarchy_top_ = &memory_hierarchy;

			/*storageUnit* ptr = memory_hierarchy_top_;
			while(ptr != NULL){
				ptr->printDetails();
				ptr = ptr->getChild();
			}*/



			/**/storageUnit* ptr = &memory_hierarchy;
			memory_hierarchy_.push_back(ptr);

			while( ptr = ptr->getChild() )
				memory_hierarchy_.push_back(ptr);


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

			/*unsigned depth = 0;
			storageUnit* ptr = memory_hierarchy_top_;
			while(ptr){
				ptr = ptr->getChild();
				depth++;
			}*/




			// clock cycle count
			unsigned long long int ccc = 0;
			unsigned long long int max_ccc = -1;

			unsigned trace_pointer = 0; // formerly "traces_processed"
			unsigned short rr_count = memory_hierarchy_[0]->getSize();
			unsigned last_level = memory_hierarchy_.size() - 1; // last level, main memory


			// initialize rr_action <trace_id, action>
			std::pair<unsigned, memAction> rr_action[rr_count];

			// initialize action queues and transfer waiting
			std::vector<std::queue< std::pair<unsigned, unsigned> >> rr_action_queue; // trace id, waiting level
			std::vector<std::queue< std::pair<unsigned, memAction> >> cache_action_queue;
			std::vector<std::pair<bool, unsigned>> transfer_waiting; // currently waiting, trace id

			// prepare action queues and transfer waiting
			for(unsigned i = 0; i < memory_hierarchy_[0]->getSize(); i++)
				rr_action_queue.push_back(std::queue< std::pair<unsigned, unsigned> >());  // trace id, level
			for(unsigned i = 0; i < rr_count; i++)
				rr_action[i] = std::make_pair(-1, VACANT);
			for(unsigned i = 0; i < last_level; i++)
				cache_action_queue.push_back(std::queue< std::pair<unsigned, memAction> >());
			for(unsigned i = 0; i < last_level; i++)
				transfer_waiting.push_back(std::make_pair(false, -1));

			// initialize latency counters
			unsigned short rr_latency_counters[rr_count]; // [rr_action_queue.size()];
			unsigned short cache_latency_counters[cache_action_queue.size()];
			bool after_wait_lock[cache_action_queue.size()];

			////
			// replace rr_latency counter and rr_action queue with simple rr_executions<trace_id, counter>[r]?

			// initialize last trace processed
			std::pair<unsigned, memAction> rr_last_trace[rr_count];
			std::pair<unsigned, memAction> cache_last_trace[cache_action_queue.size()];

			// prepare last trace processed
			for(unsigned i = 0; i < rr_count; i++)
				rr_last_trace[i] = std::make_pair(-1, IDLE);
			for(unsigned i = 0; i < cache_action_queue.size(); i++){
				cache_last_trace[i] = std::make_pair(-1, UNKNOWN);
				after_wait_lock[i] = false;
			}


			////
			/*std::cout << ">> After RRs, there are " << (last_level - 1) << " cache levels before the main memory; total of "
					  << memory_hierarchy_.size() << " <<\n\n\n";*/







			// MAIN SIMULATION LOOP
			for(; trace_pointer < trace_file.size(); ccc++){

				//std::cout << "\n>>>>>>>>6 THIS FAR\n";

				// interpret incoming trace
				unsigned mod_id, request_time, duration;
				std::tuple<unsigned, unsigned, unsigned> parsed_trace = parseTrace( trace_file[trace_pointer] );

				mod_id = std::get<0>(parsed_trace);
				request_time = std::get<1>(parsed_trace);
				duration = std::get<2>(parsed_trace);
				// /interpret


				// print clock cycle header
				std::cout << "<<< CC Count: " << ccc << " / Traces Accessed: " << trace_pointer;

				if(request_time == ccc)
					std::cout << " / NEXT REQUEST: " << trace_file[trace_pointer] << " >>>";
				else
					std::cout << " >>>";
				// /header


				//std::cout << "\n>>>>>>>>7 THIS FAR\n";




				// INITIALIZATION CASE, delegate request for next trace (0 latency action)
				if(request_time == ccc){ // if(request_time == ccc && level == 0){

					////
					std::cout << "\nThis module is ";

					unsigned target_rr = mod_id % rr_count;

					// is module already here
					if(memory_hierarchy_[0]->isModulePresent(mod_id)){

						////
						std::cout << "PRESENT in RR[" << target_rr << "] - ";

						// if not running, begin running
						if(rr_action[target_rr].second == IDLE){

							rr_action[target_rr].second = EXECUTE;
							//trace_pointer++;

							////
							std::cout << "begin EXECUTING module# " << mod_id;

						// if running, add task to RR's action queue
						}else{
							rr_action_queue[target_rr].push(std::make_pair(trace_pointer, 0));

							////
							std::cout << "can't EXECUTE because RR[" << target_rr << "] is still in use.";
						}

					// module not present, look deeper into the hierarchy
					}else{

						cache_action_queue[0].push( std::make_pair(trace_pointer, SEARCH) );

						//std::cout << "\nPUSHING TO CACHE_ACTION_QUEUE[0] TRACE# " << trace_pointer << "\n";

						////
						std::cout << "NOT PRESENT at RR[" << target_rr << "] - increase search depth (to ";
						if(last_level == 1) std::cout << "Main Memory)"; // if(level + 1 == last_level) std::cout << "Main Memory)";
						else std::cout << "L1)"; // else std::cout << "L" << (level + 1) << ")";

					}

					trace_pointer++;
				} // /init case




				//std::cout << "\n>>>>>>>>8 THIS FAR\n";


				// print states of RRs and memory level action queues
				if(true){ // if(level == 0){

					unsigned next_trace_id;
					std::tuple<unsigned, unsigned, unsigned> next_trace;

					for(unsigned short r = 0; r < rr_count; r++){

						if(r % 8 == 0)
							std::cout << "\n";
						std::cout << "RR[" << r << "]: ";

						memAction next_action = rr_action[r].second;

						//// is UNKNOWN necessary anymore?
						if(next_action != UNKNOWN && next_action != VACANT){

							next_trace_id = rr_action[r].first;
							next_trace = parseTrace( trace_file[next_trace_id] );

							std::cout << std::get<0>(next_trace);

						}else if(next_action == VACANT){
							std::cout << "⟨VACANT⟩";
						}

						switch(next_action){
							case TRANSFER: std::cout << " ⟨TRANSFERING⟩"; break;
							case EXECUTE: std::cout << " ⟨EXECUTING⟩"; break;
							case IDLE: std::cout << " ⟨IDLE⟩";
						}

						if(r + 1 != rr_count) std::cout << "    ";
					}

					std::cout << "\n\n\u2014\u2014 ACTION QUEUES \u2014\u2014\n"; // __ __
					for(unsigned short a = 0; a < 19; a++) std::cout << "\u203E"; //"\u2014"; // em dash

					// show contents of RR action queues
					for(unsigned short r = 0; r < rr_count; r++){

						/*if(r != 0 && r % 8 == 0)
							std::cout << "\n";*/
						std::cout << "\nRR[" << r << "]: ";

						std::queue<std::pair<unsigned, unsigned>> temp_queue(rr_action_queue[r]);

						for(unsigned i = 0; i < rr_action_queue[r].size(); i++){

							unsigned trace_id = temp_queue.front().first;
							std::tuple<unsigned, unsigned, unsigned> trace = parseTrace( trace_file[trace_id] );

							std::cout << "T" << trace_id << "[" << std::get<0>(trace) << "]  ";
							temp_queue.pop();
						}

					}

					// show contents of memory action queues
					for(unsigned i = 1; i <= last_level; i++){

						if(i == last_level) std::cout << "\nMain Memory:";
						else std::cout << "\nL1:"; // else std::cout << "\nL" << (level + 1) << ":";

						// print queue if it has content
						if(cache_action_queue[i - 1].size() > 0){

							std::queue< std::pair<unsigned, memAction> > temp_queue(cache_action_queue[i - 1]);

							for(unsigned j = 0; j < cache_action_queue[i - 1].size(); j++){

								unsigned next_trace_id = temp_queue.front().first;
								std::tuple<unsigned, unsigned, unsigned> next_trace = parseTrace( trace_file[next_trace_id] );

								std::cout << "  " << std::get<0>(next_trace);
								if(j == 0 && transfer_waiting[i - 1].first)
									std::cout << "...";

								temp_queue.pop();
							}

						}else{
							std::cout << "  <EMPTY SET>";
						}
					}

					std::cout << "\n\n\u2014\u2014 ACTIONS \u2014\u2014\n"; // __ __
					for(unsigned short a = 0; a < 13; a++) std::cout << "\u203E"; //"\u2014"; // em dash
					std::cout << std::endl;
				}
				// /print states



				//std::cout << "\n>>>>>>>>9 THIS FAR\n";



				// ACTIONS for RRs
				if(true){ // if(level == 0){

					for(unsigned r = 0; r < rr_count; r++){

						// only do something if there is a task for RR[r]
						if(rr_action[r].second != VACANT && rr_action[r].second != IDLE){

							unsigned next_trace_id = rr_action[r].first;
							std::tuple<unsigned, unsigned, unsigned> next_trace = parseTrace( trace_file[next_trace_id] );

							// if this is a new action, display top of latency count
							if(rr_last_trace[r].first != rr_action[r].first ||
									rr_last_trace[r].second != rr_action[r].second){

								////
								std::cout << "RR[" << r << " = " << (rr_action[r].first % rr_count) << "] - ";

								unsigned short next_latency;
								switch(rr_action[r].second){

									// attempting to receive a recently found module
									case TRANSFER:
										next_latency = rr_latency_counters[r];

										////
										std::cout << "begin TRANSFER";
										break;

									// perform task with resident module
									case EXECUTE:
										next_latency = std::get<2>(next_trace);

										////
										std::cout << "begin EXECUTION";
								}

								////
								std::cout << " of module# " << std::get<0>(next_trace) << " (duration is "
										  << next_latency << " CCs)\n";

								rr_last_trace[r] = std::make_pair(rr_action[r].first, rr_action[r].second);
							}

							////
							if(rr_latency_counters[r] - 1 != 0)
								std::cout << "RR[" << r << " = " << (rr_action[r].first % rr_count) << "] - ";

							// what is RR[r]'s current action
							switch(rr_action[r].second){

								case TRANSFER:
									rr_latency_counters[r]--;

									if(rr_latency_counters[r] != 0)
										std::cout << "TRANSFERING";
									break;

								case EXECUTE:
									rr_latency_counters[r]--;

									if(rr_latency_counters[r] != 0)
										std::cout << "EXECUTING";
							}

							////
							if(rr_latency_counters[r] != 0)
								std::cout << " module# " << std::get<0>(next_trace) << " ("
										  << rr_latency_counters[r] << " CCs remaining)\n";

							// when latency has been fully consumed, ready the next action for next cc
							if(rr_latency_counters[r] == 0){

								////
								std::cout << "RR[" << r << " = " << (rr_action[r].first % rr_count) << "] - has finished ";

								switch(rr_action[r].second){

									case TRANSFER:
										std::cout << "TRANSFERING";

										rr_latency_counters[r] = std::get<2>(parseTrace( trace_file[rr_action[r].first] ));
										rr_action[r].second = EXECUTE;
										break;

									case EXECUTE:
										std::cout << "EXECUTING";

										// if there is something in the action queue
										if(rr_action_queue[r].size() > 0){

											unsigned trace_id = rr_action_queue[r].front().first;
											unsigned target_level = rr_action_queue[r].front().second;

											rr_action[r] = std::make_pair(trace_id, TRANSFER);

											// trying to run a task that was in RR - may not be present anymore
											if(target_level == 0){

												//// todo: what to do?

											}else{

												//std::cout << "Picking up trace# " << trace_id << " from ";
												//if(target_level == last_level) std::cout << "the Main Memory.";
												//else std::cout << "L" << target_level << ".";

												// rr_latency_counters[r] = memory_hierarchy_[level]->getReadLatency();
												rr_latency_counters[r] = memory_hierarchy_[0]->getReadLatency(); //std::get<2>(parseTrace( trace_file[trace_id] ));

												//transfer_waiting[target_level - 1].first = false;
												after_wait_lock[target_level - 1] = true;


												//std::cout << "\n>>>>>>0 POPPING OFF CACHE_ACTION_QUEUE[" << (target_level - 1) << "] TRACE# "
												//		<< cache_action_queue[target_level - 1].front().first << "\n";

												// too soon to pop
												//cache_action_queue[target_level - 1].pop();
											}

											rr_action_queue[r].pop();

										// if not, relax
										}else{
											rr_action[r].second = IDLE;
										}

								}

								////
								std::cout << " module# " << std::get<0>(next_trace) << "\n";

							} // /handle spent latency

						} // handle available task


					}

				}
				// /ACTIONS for RRs


				/*std::cout << "\nFROM LEVEL 1 TO " << last_level << "\n";
				std::cout << "\nCACHE ACTION QUEUE SIZE: " << cache_action_queue.size() << "\n";
				std::cout << "\nTRANSFER WAITING SIZE: " << transfer_waiting.size() << "\n";*/

				// actions occurring this clock cycle
				for(unsigned level = 1; level <= last_level; level++){ // for(unsigned level = 0; level <= last_level; level++){

					//std::cout << "\n>>>>>>>>-5 THIS FAR\n";

					//if (level == 0){
					//}else{ // if(!after_wait_lock[level - 1]){ // if(!transfer_waiting[level].first){

					if(!cache_action_queue[level - 1].empty()){

						// only do something if there is a task for this memory level
						if(!cache_action_queue[level - 1].empty() && !transfer_waiting[level - 1].first){

							//std::cout << "\n>>>>>>>>-4 THIS FAR\n";

							////
							unsigned next_trace_id = cache_action_queue[level - 1].front().first;
							std::tuple<unsigned, unsigned, unsigned> next_trace = parseTrace( trace_file[next_trace_id] );

							//std::cout << "\n>>>>>>>>-3 THIS FAR\n";

							// if this is a new action, update latency
							if(cache_last_trace[level - 1].first != cache_action_queue[level - 1].front().first ||
									cache_last_trace[level - 1].second != cache_action_queue[level - 1].front().second){

								////
								if(level == last_level) std::cout << "Main Memory - begins ";
								else std::cout << "L" << level << " - begins ";

								unsigned short next_latency;
								switch(cache_action_queue[level - 1].front().second){

									case SEARCH:
										next_latency = memory_hierarchy_[level]->getSearchLatency();

										////
										std::cout << "SEARCH for";
										//break;

									/*case TRANSFER:
										next_latency = memory_hierarchy_[level]->getReadLatency();

										////
										std::cout << "TRANSFER of";*/
								}

								////
								std::cout << " module# " << std::get<0>(next_trace) << " (duration is "
										  << next_latency << " CCs)\n";

								// latency is the duration of the next module
								cache_latency_counters[level - 1] = next_latency;

								cache_last_trace[level - 1] = cache_action_queue[level - 1].front();
							}

							//std::cout << "\n>>>>>>>>-2 THIS FAR\n";

							////
							if(cache_latency_counters[level - 1] - 1 != 0)
								if(level == last_level) std::cout << "Main Memory - ";
								else std::cout << "L" << level << " - ";

							//std::cout << "\n>>>>>>>>-1 THIS FAR\n";

							// when waiting, no actions are taken
							if(!transfer_waiting[level - 1].first && !after_wait_lock[level - 1]){ // redundant conditional: "!transfer_waiting[level - 1].first"

								// what is this cache level's current action
								switch( cache_action_queue[level - 1].front().second ){

									// this cache level is performing a search for a module's presence
									case SEARCH:
										cache_latency_counters[level - 1]--;

										////
										if(cache_latency_counters[level - 1] != 0)
											std::cout << "SEARCHING for";
										//break;
								}

								//std::cout << "\n>>>>>>>>0 THIS FAR\n";

								////
								if(cache_latency_counters[level - 1] != 0)
									std::cout << " module# " << std::get<0>(next_trace) << " ("
											  << cache_latency_counters[level - 1] << " CCs remaining)\n";

								//std::cout << "\n>>>>>>>>1 THIS FAR\n";

								// handle fully consumed latency
								if(cache_latency_counters[level - 1] == 0){

									////
									if(level == last_level) std::cout << "Main Memory - ";
									else std::cout << "L" << level << " - ";

									////
									switch( cache_action_queue[level - 1].front().second ){

										// finished performing a search
										case SEARCH:

											////
											std::cout << "finished SEARCHING for module# " << std::get<0>(next_trace);

											// if module present, attempt to transfer to RRs
											if(memory_hierarchy_[level]->isModulePresent( std::get<0>(next_trace) )){

												////
												std::cout << " and it was FOUND - ";

												unsigned target_rr = std::get<0>(next_trace) % rr_count;

												if(rr_action[target_rr].second == VACANT || rr_action[target_rr].second == IDLE){

													rr_action[target_rr].first = next_trace_id;
													rr_action[target_rr].second = TRANSFER;

													rr_latency_counters[target_rr] = memory_hierarchy_[level]->getReadLatency();
													/*std::cout << "\n>>>>>>1 POPPING OFF CACHE_ACTION_QUEUE[" << (level - 1) << "] TRACE# "
															  << cache_action_queue[level - 1].front().first << "\n";*/
													cache_action_queue[level - 1].pop();

													////
													std::cout << " RR[" << target_rr << "] is not busy and can accept TRANSFER\n";

												}else{

													////
													std::cout << " RR[" << target_rr << "] is busy and TRANSFER must wait\n";

													transfer_waiting[level - 1] = std::make_pair(true, next_trace_id);
													//rr_action_queue[target_rr].push(std::make_pair(next_trace_id, memory_hierarchy_[level]->getReadLatency()));
													rr_action_queue[target_rr].push(std::make_pair(next_trace_id, level));
												}

											// if not present, move search to next level
											}else{

												/*std::cout << "\nPUSHING TO CACHE_ACTION_QUEUE[" << level << "] TRACE# "
														  << cache_action_queue[level - 1].front().first << "\n";*/

												cache_action_queue[level].push( std::make_pair(cache_action_queue[level - 1].front().first, SEARCH) );
												/*std::cout << "\n>>>>>>2 POPPING OFF CACHE_ACTION_QUEUE[" << (level - 1) << "] TRACE# "
														  << cache_action_queue[level - 1].front().first << "\n";*/
												cache_action_queue[level - 1].pop();

												////
												std::cout << " but it was NOT FOUND - increase search depth (to ";
												if(level + 1 == last_level) std::cout << "Main Memory)\n";
												else std::cout << "L" << (level + 1) << ")\n";
											}

											//break;

										// finished performing a transfer
										/*case TRANSFER:

											unsigned target_rr = std::get<0>(next_trace) % rr_action_queue.size();
											rr_action_queue[target_rr].push( std::make_pair(cache_action_queue[level - 1].front().first, EXECUTE) );
											cache_action_queue[level - 1].pop();

											////
											std::cout << "finished TRANSFERING to RR[" << target_rr << "] module# " << std::get<0>(next_trace) << "\n";
											break;*/
									}

								} // /handle spent latency

								//std::cout << "\n>>>>>>>>2 THIS FAR\n";

							// waiting and after wait lock has not been toggled yet
							}/*else if(!after_wait_lock[level - 1]){
								unsigned target_rr = std::get<0>(next_trace) % rr_count;

								std::cout << "waiting to begin TRANSFERING module# " << std::get<0>(next_trace)
										  << " to RR[" << target_rr << "]...";

							// handle after wait lock
							}else{

								unsigned target_rr = std::get<0>(next_trace) % rr_count;

								std::cout << "done waiting to begin TRANSFERING module# " << std::get<0>(next_trace)
										  << " to RR[" << target_rr << "].  TRANSFER will begin in next clock cycle.";

								transfer_waiting[level - 1].first = false;
								after_wait_lock[level - 1] = false;
							}*/


						} // /handle next task

						else if(!after_wait_lock[level - 1]){
							//std::cout << "\n>>>>>>>>22 THIS FAR\n";

							std::cout << "\nCACHE ACTION QUEUE SIZE: " << cache_action_queue[level - 1].size() << "\n";
							std::cout << "CACHE ACTION QUEUE FRONT: " << cache_action_queue[level - 1].front().first << "\n";

							unsigned next_trace_id = cache_action_queue[level - 1].front().first;
							std::tuple<unsigned, unsigned, unsigned> next_trace = parseTrace( trace_file[next_trace_id] );
							unsigned target_rr = std::get<0>(next_trace) % rr_count;

							if(level == last_level) std::cout << "Main Memory - ";
							else std::cout << "L" << level << " - ";

							std::cout << "waiting to begin TRANSFERING module# " << std::get<0>(next_trace) << " (T"
									  << next_trace_id << ") to RR[" << target_rr << "]...";

						// handle after wait lock
						}else{
							//std::cout << "\n>>>>>>>>33 THIS FAR\n";

							unsigned next_trace_id = cache_action_queue[level - 1].front().first;
							std::tuple<unsigned, unsigned, unsigned> next_trace = parseTrace( trace_file[next_trace_id] );
							unsigned target_rr = std::get<0>(next_trace) % rr_count;

							/*std::cout << "\nTrace data (for T" << next_trace_id << "): " << std::get<0>(next_trace) <<
									", " << std::get<1>(next_trace) << ", " << std::get<2>(next_trace) << ".\n";*/

							if(level == last_level) std::cout << "Main Memory - ";
							else std::cout << "L" << level << " - ";

							std::cout << "done waiting to begin TRANSFERING module# " << std::get<0>(next_trace)
									  << " to RR[" << target_rr << "].  TRANSFER will begin in next clock cycle.";

							/*std::cout << "\n>>>>>>0 POPPING OFF CACHE_ACTION_QUEUE[" << (level - 1) << "] TRACE# "
									  << cache_action_queue[level - 1].front().first << "\n";*/
							cache_action_queue[level - 1].pop();
							transfer_waiting[level - 1].first = false;
							after_wait_lock[level - 1] = false;

							rr_latency_counters[target_rr] = memory_hierarchy_[level]->getReadLatency();
						}



					}
					// /ACTIONS

					// prevents search overlap after a wait is completed
					if(after_wait_lock[level - 1])
						after_wait_lock[level - 1] = false;

					//std::cout << "\n>>>>>>>>3 THIS FAR\n";

				} // /cc actions loop




				//std::cout << "\n>>>>>>>>4 THIS FAR\n";



				// LEGACY DEBUG: MULTI COLUMNLAR - solid module index check; test for Li's contents
				if(ccc == stop_ccc || trace_pointer == (trace_file.size() - 1)){ // only test up to "stop_ccc" traces or the end of trace
					std::cout << "\n\n";

					/*for(int i = -1; i < memory_hierarchy_[last_level]->getSize(); i++){

						for(int j = 0; j < last_level; j++){

							// print headers
							if(i == -1){
								std::cout << "  >>> Contents of ";

								if(j == 0)
									std::cout << "RRs";
								else if(j == last_level)
									std::cout << "Main";
								else
									std::cout << "L" << j;

								if(j != last_level - 1) std::cout << " <<<\t\t";
								else std::cout << " <<<\n";

							// print modules
							}else{
								std::cout << "\tmodule " << i << " is ";

								if(memory_hierarchy_[j]->isModulePresent( i ))
									std::cout << "PRESENT @ ";
								else
									std::cout << "not present @ ";

								if(j == 0)
									std::cout << "RRs";
								else if(j == last_level)
									std::cout << "Main";
								else
									std::cout << "L" << j;

								if(j != last_level - 1) std::cout << "\t";
								else std::cout << "\n";
							}
						} // /col loop

					} // /row loop*/

					break;
				}

				//std::cout << "\n>>>>>>>>5 THIS FAR\n";



				// LEGACY DEBUG: SINGLE COLUNAR - solid module index check; test for Li's contents
				/*if(ccc == stop_ccc || trace_pointer == (trace_file.size() - 1)){ // only test up to "stop_ccc" traces or the end of trace
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
				if(ccc == 101) break;

				std::cout << "\n\n\n";

				if(ccc == max_ccc){
					std::cout << "Warning: Reached Drachma's present operational limits. Simulation ceased.";
					break;
				}
			} // end main loop
		}


	/* PRIVATE */

		//storageUnit& device::associativityToRegion(unsigned module_index);

		//storageUnit& device::findInCache(unsigned module_index);

		std::tuple<unsigned, unsigned, unsigned> device::parseTrace(std::string trace){

			unsigned mod_id, request_time, duration;

			std::string line = trace;
			std::size_t comma = line.find(",");

			mod_id = std::stol( line.substr(0, comma) );

			line = line.substr(comma + 2);
			comma = line.find(",");

			request_time = std::stol( line.substr(0, comma) );

			line = line.substr(comma + 2);
			duration = std::stol( line );

			return std::make_tuple(mod_id, request_time, duration);
		}

		//std::vector<storageUnit> pr_regions_;

		//unsigned char prr_associativity_;

		std::vector<storageUnit*> memory_hierarchy_;

		//std::vector<replAlg> repl_algs_;

#endif
