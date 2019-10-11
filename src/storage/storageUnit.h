#ifndef STORAGE_UNIT
#define STORAGE_UNIT

#include <iostream>
#include <vector>

#include "module.h"

class memoryLevel;

class storageUnit {
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

public:
	storageUnit();

	// used exclusively in the memoryLevel constructor
	storageUnit(unsigned unit_size, unsigned short read_latency, unsigned short search_latency);

	//storageUnit(std::string name, std::string file_name, unsigned short unit_size,
	//		replAlg* repl_alg, unsigned short read_latency, unsigned short search_latency);

	storageUnit(const storageUnit& su);

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
};

#endif
