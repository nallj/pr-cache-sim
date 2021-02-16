#ifndef NALLJ_DRACHMA_STORAGE_UNIT
#define NALLJ_DRACHMA_STORAGE_UNIT

#include <iostream>
#include <vector>

#include "rrModule.hpp"

class memoryLevel;

// Consider name change to "storageTier".
class storageUnit {
protected:
  // Operational Methods //
  //void updateAlg(bool is_eviction, unsigned module_index);
  //void updateAlg(bool read_hit, unsigned module_index);
  //bool hasSpace(); // move to memoryLevel
  //void evictModule(unsigned table_index); // move to memoryLevel

  unsigned unit_size_;
  //std::vector<rrModule*> module_table_;
  //replAlg* replacement_alg_; // move to memoryLevel
  memoryLevel* child_unit_;
  unsigned short read_latency_;
  unsigned short search_latency_;
  //bool read_lock_ = false;

public:
  storageUnit();

  // used exclusively in the memoryLevel constructor
  storageUnit(unsigned unit_size, unsigned short read_latency, unsigned short search_latency);

  //storageUnit(std::string name, std::string filename, unsigned short unit_size,
  //    replAlg* repl_alg, unsigned short read_latency, unsigned short search_latency);

  storageUnit(const storageUnit& su);

  virtual ~storageUnit() { }

  // Operational Methods (to be inherited) //
  virtual void printDetails() = 0;
  virtual void attemptModule(unsigned module_index) = 0;
  virtual bool isModulePresent(unsigned region_id, unsigned module_id) = 0;
  virtual void insertModule(rrModule* new_module) = 0;
  //virtual rrModule* getModule(unsigned module_id) = 0;

  // Accessors //
  unsigned short getSize();
  //std::vector<rrModule*>& getModuleTable();
  //rrModule* getModule(unsigned module_index);
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
