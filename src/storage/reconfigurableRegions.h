#ifndef _RECONFIGURABLE_REGIONS_H_
#define _RECONFIGURABLE_REGIONS_H_

#include <map>

#include "storageUnit.h"


class reconfigurableRegions : public storageUnit {

	public:
		//reconfigurableRegions();
		reconfigurableRegions() : storageUnit(0, 0, 0) { }

		//~reconfigurableRegions() {};


		// Operational Methods (inherited) //

		//void printDetails();
		void printDetails() {
			std::cout << "\t\t\t'Reconfigurable Regions'\n";
		}

		//void attemptModule(unsigned module_index);
		// never should happen unless you're trying to bring down a module (read done, now copy)
		void attemptModule(unsigned module_index){ // attempt an insert; use repl alg to decide approach

			/*if(hasSpace()){ // cache level is not full

				insertModule( new module(module_index, 777, 0) );
				//updateAlg(false, module_index);

				std::cout << "\n\tThere is room for this NEW ENTRY <" << module_index << ">\n";
				//replacement_alg_->newEntryUpdate(module_index);

			}else{ // need to replace something

				//unsigned victim_id = replacement_alg_->replace(module_index);
				//unsigned victim_module = module_table_[victim_id]->getId();

				//evictModule( victim_id );
				//insertModule( new module(module_index, 777, 0) );
				////updateAlg(true, module_index);
			}*/

		}

		//bool isModulePresent(unsigned module_index);
		/*bool isModulePresent(unsigned module_index) {

			for (unsigned i = 0; i < module_table_.size(); i++)
				if (module_table_[i]->getId() == module_index)
					return true;

			return false;
		}*/

		bool isModulePresent(unsigned region_id, unsigned module_id) {

			std::map<unsigned, module*>::iterator region_it = module_reference_table_.find(region_id);

			if (region_it != module_reference_table_.end())
				if (region_it->second->getModuleId() == module_id)
					return true;

			return false;
		}

		//void insertModule(module* new_module);
		void insertModule(module* new_module) {
			//module_table_.push_back(new_module);

			std::map<unsigned, module*>::iterator region_it
				= module_reference_table_.find(new_module->getRegionId());

			module_reference_table_[new_module->getRegionId()] = new_module;
		}


		// Operational Methods (unique) //

		//module* getModuleFromRegion(unsigned region_id);
		module* getModuleFromRegion(unsigned region_id) {
			return module_reference_table_[region_id];
		}

		bool isRegionPopulated(unsigned region_id) {
			module* target_module = getModuleFromRegion(region_id);

			return target_module->getSpeed() != 0;
		}

	private:
		std::map<unsigned, module*> module_reference_table_;
};

#endif
