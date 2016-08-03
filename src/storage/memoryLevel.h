#ifndef _MEMORY_LEVEL_H_
#define _MEMORY_LEVEL_H_

#include <map>

#include "storageUnit.h"
#include "../replAlg.h"


class memoryLevel : public storageUnit {

	public:
		//memoryLevel();
		memoryLevel() : storageUnit(0, 0, 0), replacement_alg_(NULL) { }
			//unit_size_(0), replacement_alg_(NULL), read_latency_(0), search_latency_(0),
			//child_unit_(NULL){ }

		//memoryLevel(unsigned unit_size, replAlg* repl_alg, unsigned short read_latency,
		//		unsigned short search_latency);
		// intended for main memory (automatically LUT indexed)
		memoryLevel(unsigned unit_size, replAlg* repl_alg, unsigned short read_latency,
			 unsigned short search_latency) :
				storageUnit(unit_size, read_latency, search_latency),
				replacement_alg_(repl_alg), contents_lut_indexed_(true) { }

		//memoryLevel(unsigned unit_size, replAlg* repl_alg, unsigned short read_latency,
		//		unsigned short search_latency, bool lut_indexed);
		// intended for cache levels
		memoryLevel(unsigned unit_size, replAlg* repl_alg, unsigned short read_latency,
			 unsigned short search_latency, bool lut_indexed) :
				storageUnit(unit_size, read_latency, search_latency),
				replacement_alg_(repl_alg), contents_lut_indexed_(lut_indexed) { }
			//unit_size_(unit_size), replacement_alg_(repl_alg), read_latency_(read_latency),
			//search_latency_(search_latency), child_unit_(NULL){ // , parent_unit_(NULL)

		memoryLevel(const storageUnit& su) : storageUnit(su) { }


		// Operational Methods (inherited) //

		//void printDetails();
		void printDetails(){

			std::cout << "\t\t\t'" << name_ << "'\n"
					  << "\t\t\t\tParameters: size(" << unit_size_ << "), read latency(" << read_latency_
					  << "), search latency(" << search_latency_ << ")\n";

			if(replacement_alg_) replacement_alg_->printName();
		}

		//void attemptModule(unsigned module_index);
		// never should happen unless you're trying to bring down a module (read done, now copy)
		void attemptModule(unsigned module_index){ // attempt an insert; use repl alg to decide approach

			/*if(hasSpace()){ // cache level is not full

				insertModule( new module(module_index, 777, 0) );
				//updateAlg(false, module_index);

				std::cout << "\n\tThere is room for this NEW ENTRY <" << module_index << ">\n";
				replacement_alg_->newEntryUpdate(module_index);

			}else{ // need to replace something

				unsigned victim_id = replacement_alg_->replace(module_index);
				unsigned victim_module = module_table_[victim_id]->getId();

				evictModule( victim_id );
				insertModule( new module(module_index, 777, 0) );
				//updateAlg(true, module_index);
			}*/

		}

		//bool isModulePresent(unsigned module_index);
		/*bool isModulePresent(unsigned module_index){

			for(unsigned i = 0; i < module_table_.size(); i++)
				if(module_table_[i]->getId() == module_index)
					return true;

			return false;
		}*/

		bool isModulePresent(unsigned region_id,unsigned module_id) {

			std::map<unsigned, std::map<unsigned, module*>>::iterator region_it
				= module_reference_table_.find(region_id);

			if (region_it != module_reference_table_.end()) {

				std::map<unsigned, module*>::iterator module_it
					= region_it->second.find(module_id);

				if (module_it != region_it->second.end())
					return true;
			}

			return false;
		}

		//void insertModule(module* new_module);
		void insertModule(module* new_module) {

			unsigned region_id = new_module->getRegionId();
			unsigned module_id = new_module->getModuleId();

			std::map<unsigned, std::map<unsigned, module*>>::iterator region_it
				= module_reference_table_.find(region_id);

			if (region_it == module_reference_table_.end())
				module_reference_table_[region_id][module_id] = new_module;

			else
				region_it->second[module_id] = new_module;
		}


		// Operational Methods (unique) //
		bool isLutIndexed() { return contents_lut_indexed_; };

		module* getModule(unsigned region_id, unsigned module_id) {
			return module_reference_table_[region_id][module_id];
		}

		bool hasSpace() {
			return false;
		}

		void evictModule(unsigned table_index) {
			// do eviction stuff here
		}

	private:
		replAlg* replacement_alg_;
		bool contents_lut_indexed_;

		std::map<unsigned, std::map<unsigned, module*>> module_reference_table_;

};

#endif
