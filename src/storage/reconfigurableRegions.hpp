#ifndef RECONFIGURABLE_REGIONS
#define RECONFIGURABLE_REGIONS

#include <map>

#include "storageUnit.hpp"

class reconfigurableRegions : public storageUnit {

  std::map<unsigned, module*> module_reference_table_;

public:
  reconfigurableRegions();

  // Operational Methods (inherited) //
  void printDetails();
  bool isModulePresent(unsigned region_id, unsigned module_id);
  void insertModule(module* new_module);

  // Legacy (inherited)
  void attemptModule(unsigned module_index);

  // Operational Methods (unique) //
  module* getModuleFromRegion(unsigned region_id);
  bool isRegionPopulated(unsigned region_id);
};

#endif
