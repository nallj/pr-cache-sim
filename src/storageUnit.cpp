#ifndef _STORAGE_UNIT_
#define _STORAGE_UNIT_

#include "storageUnit.h"


/* PUBLIC */
	storageUnit::storageUnit(unsigned short unit_size, unsigned short int read_latency, unsigned short int search_latency) :
		unit_size_(unit_size), read_latency_(read_latency), search_latency_(search_latency){

		/* storageUnit* child_unit_;*/
	}


	void storageUnit::printDetails(){
		std::cout << "This is a memory object known as '" << name_ << "'\n"
				  << "\tPARAMS: size(" << unit_size_ << "), read latency(" << read_latency_
				  << "), search latency(" << search_latency_ << ")\n";
	}

	//bool isModulePresent(unsigned module_index);

	//bool hasSpace(module& new_module);

	//void evictModule(unsigned module_index);


	unsigned short storageUnit::getSize(){ return unit_size_; }

	//std::vector<module*>& getModuleTable();

	//module& getModule(unsigned module_index);

	storageUnit* storageUnit::getChild(){ return child_unit_; }

	unsigned short int storageUnit::getReadLatency(){ return read_latency_; }

	unsigned short int storageUnit::getSearchLatency(){ return search_latency_; }

	//bool availableToRead();


	//void insertModule(module& new_module);

	void storageUnit::setChild(storageUnit* child_level){ child_unit_ = child_level; }

	//void toggleReadLock();


/* PRIVATE */
	//unsigned short unit_size_;

	//std::vector<module*> module_table_;

	//storageUnit& child_unit_;

	//unsigned short int read_latency_;

	//unsigned short int search_latency_;

	//bool read_lock_;

#endif
