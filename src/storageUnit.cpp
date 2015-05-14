#ifndef _STORAGE_UNIT_
#define _STORAGE_UNIT_

#include "storageUnit.h"


/* PUBLIC */

	storageUnit::storageUnit(unsigned short unit_size, replAlg* repl_alg, unsigned short int read_latency, unsigned short int search_latency) :
		unit_size_(unit_size), replacement_alg_(repl_alg), read_latency_(read_latency), search_latency_(search_latency), child_unit_(NULL){

		//for(unsigned i = 0; i < unit_size; i++)
		//	module_table_.push_back(new module(i, i*3));
	}


	void storageUnit::printDetails(){

		std::cout << "This is a memory object known as '" << name_ << "'\n"
				  << "\tPARAMS: size(" << unit_size_ << "), read latency(" << read_latency_
				  << "), search latency(" << search_latency_ << ")\n";

		if(replacement_alg_){
			std::cout << "\t";
			replacement_alg_->printName();
		}
	}

	void storageUnit::attemptModule(unsigned module_index){

		// cache HIT, simply update alg table
		if(isModulePresent(module_index)) updateAlg(true, module_index);

		// cache MISS, either insert or replace
		else{

			if(hasSpace()){ // lower memory is not full

				insertModule( new module(module_index, 777) );
				updateAlg(false, module_index);

			}else{ // need to replace something

				evictModule( replacement_alg_->replace(module_index) );
				insertModule( new module(module_index, 777) );
			}
		}

	}

	bool storageUnit::isModulePresent(unsigned module_index){

		// still BAD??????????
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

	storageUnit* storageUnit::getChild(){ return child_unit_; }

	unsigned short int storageUnit::getReadLatency(){ return read_latency_; }

	unsigned short int storageUnit::getSearchLatency(){ return search_latency_; }

	bool storageUnit::availableToRead(){ return read_lock_; }


	void storageUnit::setChild(storageUnit* child_level){ child_unit_ = child_level; }

	void storageUnit::toggleReadLock(){}


/* PRIVATE */

	void storageUnit::updateAlg(bool read_hit, unsigned module_index){

		if(read_hit){

			std::cout << "READ HIT <" << module_index << ">\n\n";
			replacement_alg_->readHitUpdate(module_index); // if present, simply update alg table

		}else{

			std::cout << "READ MISS, there is space - NEW ENTRY <" << module_index << ">\n";
			replacement_alg_->newEntryUpdate(module_index);
		}

	}

	bool storageUnit::hasSpace(){
		return (unit_size_ != module_table_.size());
	}

	void storageUnit::evictModule(unsigned table_index){
		module_table_.erase(module_table_.begin() + table_index);
	}

	//unsigned short unit_size_;

	//std::vector<module*> module_table_;

	//replAlg* replacement_alg_;

	//storageUnit& child_unit_;

	//unsigned short int read_latency_;

	//unsigned short int search_latency_;

	//bool read_lock_;


#endif
