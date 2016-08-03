#ifndef _DEVICE_
#define _DEVICE_

#include "device.h"


	/* PUBLIC */

		void device::associateHierarchy(reconfigurableRegions memory_hierarchy) {
			//memory_hierarchy_top_ = &memory_hierarchy;
			memory_hierarchy_top_ = memory_hierarchy;

			//storageUnit* ptr = &memory_hierarchy;
			storageUnit* ptr = &memory_hierarchy;
			memory_hierarchy_.push_back(ptr);

			while ( ptr = ptr->getChild() )
				memory_hierarchy_.push_back(ptr);
		}

		bool device::prepareApplicationResources(application* app) {
			//simulated_application_ = app;

			// instantiate all static regions with their module contents
			std::multimap<unsigned, unsigned>* static_module_counts = app->getStaticModules();

			// iterate through each static region
			std::multimap<unsigned, unsigned>::iterator static_it = static_module_counts->begin();
			for (; static_it != static_module_counts->end(); static_it++) {

				unsigned region_id = static_it->first;
				std::vector<module*> current_static_modules;

				// todo: consider allowing for each static region to have its' own speed

				// iterate through each module in the target static region
				for (unsigned i = 0; i < static_it->second; i++)
					current_static_modules.push_back(new module(i, 0, app->getStaticRegionSpeed()));

				static_regions_.insert(std::pair<unsigned, std::vector<module*>>(region_id, current_static_modules));
			}

			// instantiate all reconfigurable regions with their module contents
			std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>* reconfig_module_data
				= app->getReconfigurableModules();

			// iterate through each reconfigurable region; for each region, create a placeholder module
			unsigned required_bitstream_space = 0;
			std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>::iterator reconfig_it
				= reconfig_module_data->begin();

			// for each reconfigurable module, add it to the bitstream library in main memory
			storageUnit* main_memory = memory_hierarchy_.back();

			for (; reconfig_it != reconfig_module_data->end(); reconfig_it++) {

				unsigned region_id = reconfig_it->first;
				unsigned bitstream_width = reconfig_it->second.first;

				// at the PRR level, insert a placeholder module
				module* new_mod = new module(region_id, -1, 0, 0);
				memory_hierarchy_top_.insertModule(new_mod);

				// track the amount of space required for the main memory to store all reconfigurable modules
				unsigned region_module_count = reconfig_it->second.second.size();
				required_bitstream_space += region_module_count * bitstream_width;

				// document the amount of modules in each region for later use
				prr_census_.insert(std::pair<unsigned, unsigned>(region_id, region_module_count));
				// was prr_index_

				// instantiate the bitstream library as modules are iterated over
				for (unsigned i = 0; i < region_module_count; i++) {

					module* bitstream = new module(region_id, i, bitstream_width, reconfig_it->second.second[i]);
					main_memory->insertModule(bitstream);
				}
			}

			// todo: check each cache level and warn the user if there is not enough space for
			// certain modules; fail if a level cant even hold one module

			prr_bitstream_sizes_ = app->getReconfigurableRegionBitstreamSizes();
			simulator_speed_ = app->getSimulatorSpeed();

			icap_ = app->getIcap();
			prc_ = app->getPrc();

			// return true if the bitstream library can fit in the chosen main memory
			return main_memory->getSize() >= required_bitstream_space;
		}

		void device::parseTraceFile(std::vector<std::string> trace_file) {
			traces_ = new std::vector<traceToken*>();

			for (unsigned i = 0; i < trace_file.size(); i++)
				traces_->push_back(new traceToken(trace_file[i]));
		}

		void device::simulateApplication(unsigned long long int stop_ccc) {

			// clock cycle count
			unsigned long long int ccc = 0;
			unsigned long long int max_ccc = -1;

			unsigned trace_pointer = 0;
			//unsigned short rr_count = memory_hierarchy_[0]->getSize();
			unsigned last_level = memory_hierarchy_.size() - 1; // last level, main memory


			// initialize rr_action <trace_id, action>
			//std::pair<unsigned, memAction> rr_action[rr_count];

			// initialize action queues and transfer waiting
			std::vector<std::queue< std::pair<unsigned, unsigned> >> rr_action_queue; // trace id, waiting level
			std::vector<std::queue< std::pair<unsigned, memAction> >> cache_action_queue;
			std::vector<std::pair<bool, unsigned>> transfer_waiting; // currently waiting, trace id

			// prepare action queues and transfer waiting
			for(unsigned i = 0; i < memory_hierarchy_[0]->getSize(); i++)
				rr_action_queue.push_back(std::queue< std::pair<unsigned, unsigned> >());  // trace id, level
			//for(unsigned i = 0; i < rr_count; i++)
			//	rr_action[i] = std::make_pair(-1, VACANT);
			for(unsigned i = 0; i < last_level; i++)
				cache_action_queue.push_back(std::queue< std::pair<unsigned, memAction> >());
			for(unsigned i = 0; i < last_level; i++)
				transfer_waiting.push_back(std::make_pair(false, -1));

			// initialize latency counters
			//unsigned short rr_latency_counters[rr_count]; // [rr_action_queue.size()];
			unsigned short cache_latency_counters[cache_action_queue.size()];
			bool after_wait_lock[cache_action_queue.size()];

			////
			// replace rr_latency counter and rr_action queue with simple rr_executions<trace_id, counter>[r]?

			// initialize last trace processed
			//std::pair<unsigned, memAction> rr_last_trace[rr_count];
			std::pair<unsigned, memAction> cache_last_trace[cache_action_queue.size()];

			// prepare last trace processed
			//for(unsigned i = 0; i < rr_count; i++)
			//	rr_last_trace[i] = std::make_pair(-1, IDLE);
			for(unsigned i = 0; i < cache_action_queue.size(); i++) {
				cache_last_trace[i] = std::make_pair(-1, UNKNOWN);
				after_wait_lock[i] = false;
			}


			////
			/*std::cout << ">> After RRs, there are " << (last_level - 1) << " cache levels before the main memory; total of "
					  << memory_hierarchy_.size() << " <<\n\n\n";*/








			//applicationScheduler app_scheduler = applicationScheduler(trace_file);

			std::vector< std::queue<traceToken*>* > static_region_action_queue;
			std::vector< std::queue<traceToken*>* > prr_action_queue; // trace id, waiting level
			std::vector< std::queue<unsigned> > memory_search_queues;

			for (unsigned i = 0; i < static_regions_.size(); i++)
				static_region_action_queue.push_back(new std::queue<traceToken*>);

			///////////////////////////////////////
			// for some reason memory_hierarchy_[0] is not populated.. see prepareApplicationResources()
			//unsigned short prr_count = memory_hierarchy_[0]->getSize();
			//for(unsigned i = 0; i < memory_hierarchy_[0]->getSize(); i++)

			unsigned short prr_count = prr_census_.size();// simulated_application_->getReconfigurableRegionCount();
			for(unsigned i = 0; i < prr_count; i++)
				prr_action_queue.push_back(new std::queue<traceToken*>());  // trace id, level



			for(unsigned i = 0; i < last_level; i++)
				memory_search_queues.push_back(std::queue<unsigned>());

			//unsigned short prr_count = memory_hierarchy_[0]->getSize();
			std::pair<unsigned, bool> prr_contents[prr_count]; // trace id, currently executing (false = IDLE)

			// retrieve the speed of the fastest module and set that as the simulator's operating speed.
			//double simulator_speed = simulated_application_->getSimulatorSpeed();
			std::cout << "\nINFO: Simulator will execute at a rate of " << simulator_speed_ << " MHz.\n";





			// create the PRR level controllers
			std::vector<prrLevelController*> prr_level_controllers;
			for (unsigned i = 0; i < prr_count; i++)
				prr_level_controllers.push_back(
					new prrLevelController(i, &memory_hierarchy_top_, &memory_hierarchy_, simulator_speed_));
			// todo: prrLevelController needs access to its' PRR and the rest of the hierarchy

			// create a signal context which will be used to "parallelize" the simulation actions.
			signalContext simulation_context = signalContext(&prr_level_controllers, prc_, icap_);

			// wire up the PRR controllers
			//for (unsigned i = 0; i < prr_count; i++)
			//	prr_level_controllers[i]->connect(simulation_context.accessContextSignal(ICAP_PRR_REQ, i));

			// wire up the PRC
			prc_->connect(&memory_hierarchy_, &memory_hierarchy_top_, traces_,
						  simulation_context.accessContextSignalBus(PRR_EXE),
						  simulation_context.accessContextSignal(PRR_PRC_ACK),
					      simulation_context.accessContextSignal(ICAP_PRC_ACK),
						  simulation_context.accessContextSignal(ICAP_TRANSFER_PRR),
						  simulation_context.accessContextCurrentTrace(false));

			// wire up the ICAP
			icap_->connect(&memory_hierarchy_, &memory_hierarchy_top_,
						   simulation_context.accessContextSignalBus(PRR_EXE),
						   simulation_context.accessContextCurrentTrace(true),
						   simulation_context.accessContextCounterSignal(PRC_MC),
						   simulation_context.accessContextSignal(PRC_ICAP_REQ),
						   simulation_context.accessContextSignal(PRR_ICAP_ACK));

			// inform the ICAP of the bitstream sizes for each of the reconfigurable regions
			icap_->setRegionWidths(prr_bitstream_sizes_);
			icap_->setSimulationSpeed(simulator_speed_);

			// wire up the PRR controllers
			for (unsigned i = 0; i < prr_count; i++)
				prr_level_controllers[i]->connect(simulation_context.accessContextSignal(PRC_START_PRR, i),
					simulation_context.accessContextSignal(PRC_ENQUEUE_PRR, i),
					simulation_context.accessContextCounterSignal(ICAP_MC),
					simulation_context.accessContextSignal(ICAP_PRR_REQ, i),
					simulation_context.accessContextSignal(ICAP_TRANSFER_PRR),
					simulation_context.accessContextCurrentTrace(true),
					simulation_context.accessContextCurrentTrace(false));




			// MAIN SIMULATION LOOP
			for (; trace_pointer < traces_->size(); ccc++) {


				// readability divider
				std::cout << "\n------------------------------------------------------------------------------------------------------------\n";

				// PRC and ICAP step
				prc_->step();
				icap_->step();

				std::cout << std::endl;

				// PRR controllers step
				for (unsigned i = 0; i < prr_level_controllers.size(); i++)
					prr_level_controllers[i]->step();

				// propagate all signal values through the simulation signal context
				simulation_context.refreshContext(true);





				//std::cout << std::endl;

				if(ccc == max_ccc){
					std::cout << "Warning: Reached Drachma's present operational limits. Simulation ceased.";
					break;
				}

				// stop at requested stop point
				if (ccc == stop_ccc)
					break;

				// safety stop
				if(ccc == 1000) break;

			} // end main loop
		}


	/* PRIVATE */

		/*//storageUnit& device::associativityToRegion(unsigned module_index);

		//storageUnit& device::findInCache(unsigned module_index);

		//std::tuple<unsigned, unsigned, unsigned> device::parseTrace(std::string trace) {

		//	unsigned mod_id, request_time, duration;

			/*std::string line = trace;
			std::size_t comma = line.find(",");

			mod_id = std::stol( line.substr(0, comma) );

			line = line.substr(comma + 2);
			comma = line.find(",");

			request_time = std::stol( line.substr(0, comma) );

			line = line.substr(comma + 2);
			duration = std::stol( line );*/

		//	return std::make_tuple(mod_id, request_time, duration);
		//}*/

		/*module* device::getBitstreamFromLibrary(unsigned region_id, unsigned module_id) {

			unsigned adjusted_module_id = 0;
			for (unsigned i = 0; i < region_id; i++)
				adjusted_module_id += prr_index_.at(i);

			adjusted_module_id += module_id;

			return memory_hierarchy_.back()->getModule(adjusted_module_id);
		}*/


		/*//std::vector<storageUnit> pr_regions_;

		//unsigned char prr_associativity_;

		//std::vector<storageUnit*> memory_hierarchy_;

		//std::vector<replAlg> repl_algs_;*/

#endif
