/*#ifndef _RECONFIGURABLE_REGIONS_
#define _RECONFIGURABLE_REGIONS_

#include "reconfigurableRegions.h"


/* PUBLIC */
	//storageUnit(unsigned unit_size, replAlg* repl_alg,
	//			unsigned short read_latency, unsigned short search_latency);
/*
	reconfigurableRegions::reconfigurableRegions() : storageUnit(0, NULL, 0, 0) { }


	// Operational Methods (inherited) //

	void reconfigurableRegions::printDetails(){

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
	void reconfigurableRegions::attemptModule(unsigned module_index){ // attempt an insert; use repl alg to decide approach

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

	bool reconfigurableRegions::isModulePresent(unsigned module_index) {

		for (unsigned i = 0; i < module_table_.size(); i++)
			if (module_table_[i]->getId() == module_index)
				return true;

		return false;
	}

	void reconfigurableRegions::insertModule(module* new_module) {
		module_table_.push_back(new_module);
	}


	// Operational Methods (unique) //

	module* reconfigurableRegions::getModuleFromRegion(unsigned region_id) {
		return module_table_[region_id];
	}

#endif
*/
