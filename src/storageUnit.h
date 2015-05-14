#ifndef _STORAGE_UNIT_H_
#define _STORAGE_UNIT_H_

#include <iostream>
#include <vector>
//#include <utility>

#include "module.h"
#include "replAlg.h"


class storageUnit{

	public:
		storageUnit(unsigned short unit_size, replAlg* repl_alg, unsigned short int read_latency, unsigned short int search_latency);


		// Operational Methods //
		void printDetails();

		void attemptModule(unsigned module_index);

		bool isModulePresent(unsigned module_index);

		void insertModule(module* new_module);


		// Getters //
		unsigned short getSize();

		std::vector<module*>& getModuleTable();

		module* getModule(unsigned module_index);

		storageUnit* getChild();

		unsigned short int getReadLatency();

		unsigned short int getSearchLatency();

		bool availableToRead();


		// Mutators //
		void setChild(storageUnit* child_level);

		void toggleReadLock();


		std::string name_;

	private:
		// Operational Methods //
		void updateAlg(bool read_hit, unsigned module_index);

		bool hasSpace();

		void evictModule(unsigned table_index);


		unsigned short unit_size_;

		std::vector<module*> module_table_;

		replAlg* replacement_alg_;

		storageUnit* child_unit_;

		unsigned short int read_latency_;

		unsigned short int search_latency_;

		bool read_lock_ = false;
};

#endif
