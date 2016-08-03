#ifndef _STORAGE_UNIT_H_
#define _STORAGE_UNIT_H_

/*#include <iostream>
#include <vector>
//#include <utility>

#include "module.h"
#include "replAlg.h"


class storageUnit{

	public:
		storageUnit();

		storageUnit(unsigned short rc_reg_count);

		storageUnit(unsigned short unit_size, replAlg* repl_alg, unsigned short int read_latency, unsigned short int search_latency);

		storageUnit(std::string name, std::string file_name, unsigned short unit_size, replAlg* repl_alg, unsigned short int read_latency, unsigned short int search_latency);


		// Operational Methods //
		void printDetails();

		void attemptModule(unsigned module_index);

		bool isModulePresent(unsigned module_index);

		void insertModule(module* new_module);


		// Accessors //
		unsigned short getSize();

		std::vector<module*>& getModuleTable();

		module* getModule(unsigned module_index);

		storageUnit* getParent();

		storageUnit* getChild();

		unsigned short int getReadLatency();

		unsigned short int getSearchLatency();

		bool availableToRead();


		// Mutators //
		void setParent(storageUnit* parent_level);

		void setChild(storageUnit* child_level);

		void toggleReadLock();


		std::string name_, file_;

	private:
		// Operational Methods //
		//void updateAlg(bool is_eviction, unsigned module_index);
		//void updateAlg(bool read_hit, unsigned module_index);

		bool hasSpace();

		void evictModule(unsigned table_index);


		unsigned short unit_size_;

		std::vector<module*> module_table_;

		replAlg* replacement_alg_;

		//storageUnit* parent_unit_;
		storageUnit* child_unit_;

		unsigned short int read_latency_;

		unsigned short int search_latency_;

		bool read_lock_ = false;
};*/

#include <iostream>
#include <vector>
//#include <utility>

//#include "memoryLevel.h"
#include "module.h"
//////////////////////#include "../replAlg.h"


// handle circular dependency
class memoryLevel;

class storageUnit {

	public:
		storageUnit();

		// used exclusively in the memoryLevel constructor
		storageUnit(unsigned unit_size, unsigned short read_latency, unsigned short search_latency);

		//storageUnit(std::string name, std::string file_name, unsigned short unit_size,
		//		replAlg* repl_alg, unsigned short read_latency, unsigned short search_latency);

		storageUnit(const storageUnit& su) { *this = su; }

		virtual ~storageUnit() { }


		// Operational Methods (to be inherited) //
		virtual void printDetails() = 0;

		virtual void attemptModule(unsigned module_index) = 0;

		virtual bool isModulePresent(unsigned region_id, unsigned module_id) = 0;

		virtual void insertModule(module* new_module) = 0;

		//virtual module* getModule(unsigned module_id) = 0;


		// Accessors //
		unsigned short getSize();

		//std::vector<module*>& getModuleTable();

		//module* getModule(unsigned module_index);

		memoryLevel* getChild();

		unsigned short getReadLatency();

		unsigned short getSearchLatency();

		bool availableToRead();


		// Mutators //
		void setChild(memoryLevel* child_level);

		//void toggleReadLock();


		std::string name_, file_;

	protected:
		// Operational Methods //
		//void updateAlg(bool is_eviction, unsigned module_index);
		//void updateAlg(bool read_hit, unsigned module_index);

		//bool hasSpace(); // move to memoryLevel

		//void evictModule(unsigned table_index); // move to memoryLevel


		unsigned unit_size_;

		//std::vector<module*> module_table_;

		//replAlg* replacement_alg_; // move to memoryLevel

		memoryLevel* child_unit_;

		unsigned short read_latency_;

		unsigned short search_latency_;

		//bool read_lock_ = false;
};

#endif
