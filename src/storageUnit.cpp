#ifndef _STORAGE_UNIT_
#define _STORAGE_UNIT_

#include "storageUnit.h"


/* PUBLIC */
	storageUnit::storageUnit(unsigned short rc_reg_count) :
		unit_size_(rc_reg_count), replacement_alg_(NULL), read_latency_(0), search_latency_(0),
		child_unit_(NULL){ } // , parent_unit_(NULL), name_("Reconfigurable Regions")

	storageUnit::storageUnit(unsigned short unit_size, replAlg* repl_alg, unsigned short int read_latency, unsigned short int search_latency) :
		unit_size_(unit_size), replacement_alg_(repl_alg), read_latency_(read_latency),
		search_latency_(search_latency), child_unit_(NULL){ // , parent_unit_(NULL)

		//for(unsigned i = 0; i < unit_size; i++)
		//	module_table_.push_back(new module(i, i*3));
	}


	void storageUnit::printDetails(){

		if(replacement_alg_ == NULL && read_latency_ == 0 && search_latency_ == 0)
			std::cout << "\t\t\t'Reconfigurable Regions'\n\t\t\t\tParameter: size(" << unit_size_ << ")\n";
		else
			std::cout << "\t\t\t'" << name_ << "'\n"
				  	  << "\t\t\t\tParameters: size(" << unit_size_ << "), read latency(" << read_latency_
					  << "), search latency(" << search_latency_ << ")\n";

		if(replacement_alg_) replacement_alg_->printName();
	}

	// never should happen unless you're trying to bring down a module (read done, now copy)
	void storageUnit::attemptModule(unsigned module_index){ // attempt an insert; use repl alg to decide approach

		if(hasSpace()){ // cache level is not full

			insertModule( new module(module_index, 777) );
			//updateAlg(false, module_index);

			std::cout << "\n\tThere is room for this NEW ENTRY <" << module_index << ">\n";
			replacement_alg_->newEntryUpdate(module_index);

		}else{ // need to replace something

			unsigned victim_id = replacement_alg_->replace(module_index);
			unsigned victim_module = module_table_[victim_id]->getId();

			evictModule( victim_id );
			insertModule( new module(module_index, 777) );
			//updateAlg(true, module_index);

			// todo: is parent pointer necessary?
			/*storageUnit* parent = getParent();
			// send victim to next cache level to be evaluated for insertion
			if(parent && !parent->isModulePresent(victim_module))
				getParent()->attemptModule(victim_module);
				// if an eviction occurs, need to attemptModule on next cache level*/
		}

	}

	bool storageUnit::isModulePresent(unsigned module_index){

		for(unsigned i = 0; i < module_table_.size(); i++)
			if(module_table_[i]->getId() == module_index)
				return true;

		return false;
	}

	void storageUnit::insertModule(module* new_module){
		module_table_.push_back(new_module);
	}


	unsigned short storageUnit::getSize(){ return unit_size_; }

	std::vector<module*>& storageUnit::getModuleTable(){
		return module_table_;
	}

	module* storageUnit::getModule(unsigned module_index){
		return module_table_[module_index];
	}

	//storageUnit* storageUnit::getParent(){ return parent_unit_; }

	storageUnit* storageUnit::getChild(){ return child_unit_; }

	unsigned short int storageUnit::getReadLatency(){ return read_latency_; }

	unsigned short int storageUnit::getSearchLatency(){ return search_latency_; }

	bool storageUnit::availableToRead(){ return !read_lock_; }

	//void storageUnit::setParent(storageUnit* parent_level){ parent_unit_ = parent_level; }

	void storageUnit::setChild(storageUnit* child_level){ child_unit_ = child_level; }

	void storageUnit::toggleReadLock(){ read_lock_ = !read_lock_; }


/* PRIVATE */

	/*void storageUnit::updateAlg(bool do_replace, unsigned module_index){

			if(do_replace){

				//std::cout << "\n\tREAD HIT <" << module_index << ">\n";
				//replacement_alg_->readHitUpdate(module_index); // if present, simply update alg table
				replacement_alg_->replaceUpdate(module_index);

			}else{

				//std::cout << "\n\tREAD MISS, there is space - NEW ENTRY <" << module_index << ">\n";
				std::cout << "\n\tThere is room for this NEW ENTRY <" << module_index << ">\n";
				replacement_alg_->newEntryUpdate(module_index);
			}

		}*/

	/*void storageUnit::updateAlg(bool read_hit, unsigned module_index){

		if(read_hit){

			std::cout << "\n\tREAD HIT <" << module_index << ">\n";
			replacement_alg_->readHitUpdate(module_index); // if present, simply update alg table

		}else{

			std::cout << "\n\tREAD MISS, there is space - NEW ENTRY <" << module_index << ">\n";
			replacement_alg_->newEntryUpdate(module_index);
		}

	}*/

	bool storageUnit::hasSpace(){
		return (unit_size_ != module_table_.size());
	}

	void storageUnit::evictModule(unsigned table_index){
		module_table_.erase(module_table_.begin() + table_index);
	}

	//unsigned short unit_size_;

	//std::vector<module*> module_table_;

	//replAlg* replacement_alg_;

	//storageUnit* parent_unit_;

	//unsigned short int read_latency_;

	//unsigned short int search_latency_;

	//bool read_lock_;


#endif
