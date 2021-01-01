#ifndef NALLJ_DRACHMA_RECONFIGURABLE_REGIONS
#define NALLJ_DRACHMA_RECONFIGURABLE_REGIONS

#include <map>

#include "storageUnit.hpp"

class reconfigurableRegions : public storageUnit {

  std::map<unsigned, rrModule*> module_reference_table_;

public:
  reconfigurableRegions();

  // Operational Methods (inherited) //
  void printDetails();
  bool isModulePresent(unsigned region_id, unsigned module_id);
  void insertModule(rrModule* new_module);

  // Legacy (inherited)
  void attemptModule(unsigned module_index);

  // Operational Methods (unique) //
  rrModule* getModuleFromRegion(unsigned region_id);
  bool isRegionPopulated(unsigned region_id);
};

#endif
