/*#ifndef _MEMORY_LEVEL_
#define _MEMORY_LEVEL_

#include "memoryLevel.h"


/* PUBLIC */
/*	memoryLevel::memoryLevel() : storageUnit(0, NULL, 0, 0) { }
		//unit_size_(0), replacement_alg_(NULL), read_latency_(0), search_latency_(0),
		//child_unit_(NULL){ }

	memoryLevel::memoryLevel(unsigned unit_size, replAlg* repl_alg, unsigned short read_latency,
		 unsigned short search_latency) :
			storageUnit(unit_size, repl_alg, read_latency, search_latency),
			contents_lut_indexed_(true) {}

	memoryLevel::memoryLevel(unsigned unit_size, replAlg* repl_alg, unsigned short read_latency,
		 unsigned short search_latency, bool lut_indexed) :
			storageUnit(unit_size, repl_alg, read_latency, search_latency),
			contents_lut_indexed_(lut_indexed) {}
		//unit_size_(unit_size), replacement_alg_(repl_alg), read_latency_(read_latency),
		//search_latency_(search_latency), child_unit_(NULL){ // , parent_unit_(NULL)


	void memoryLevel::printDetails(){

		if(replacement_alg_ == NULL && read_latency_ == 0 && search_latency_ == 0)
			//std::cout << "\t\t\t'Reconfigurable Regions'\n\t\t\t\tParameter: size(" << unit_size_ << ")\n";'
			std::cout << "\t\t\t'Reconfigurable Regions'\n";
		else
			std::cout << "\t\t\t'" << name_ << "'\n"
				  	  << "\t\t\t\tParameters: size(" << unit_size_ << "), read latency(" << read_latency_
					  << "), search latency(" << search_latency_ << ")\n";

		if(replacement_alg_) replacement_alg_->printName();
	}

	// never should happen unless you're trying to bring down a module (read done, now copy)
	void memoryLevel::attemptModule(unsigned module_index){ // attempt an insert; use repl alg to decide approach

		if(hasSpace()){ // cache level is not full

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
		}

	}

	bool memoryLevel::isModulePresent(unsigned module_index){

		for(unsigned i = 0; i < module_table_.size(); i++)
			if(module_table_[i]->getId() == module_index)
				return true;

		return false;
	}

	void memoryLevel::insertModule(module* new_module){
		module_table_.push_back(new_module);
	}

#endif
*/
