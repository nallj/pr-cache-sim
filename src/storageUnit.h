#ifndef _STORAGE_UNIT_H_
#define _STORAGE_UNIT_H_

#include <string>
#include <vector>
#include <utility>

#include "module.h"

class storageUnit{

	public:
		storageUnit(unsigned short unit_size, unsigned short int read_latency, unsigned short int search_latency);

		unsigned short getSize();

		std::vector<module*>& getModuleTable();

		module& getModule(unsigned module_index);

		storageUnit& getChild();

		unsigned short int getReadLatency();

		unsigned short int getSearchLatency();

		bool isModulePresent(unsigned module_index);

		bool hasSpace(module& new_module);

		void insertModule(module& new_module);

		void evictModule(unsigned module_index);

		bool availableToRead();

		void toggleReadLock();

		void setChild(storageUnit& child_level);

		std::string name_;

	private:
		unsigned short unit_size_;

		std::vector<module*> module_table_;

		storageUnit& child_unit_;

		unsigned short int read_latency_;

		unsigned short int search_latency_;

		bool read_lock_;
};

#endif
