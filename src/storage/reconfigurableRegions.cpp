#include "reconfigurableRegions.hpp"

reconfigurableRegions::reconfigurableRegions() : storageUnit(0, 0, 0) { }


// Operational Methods (inherited) //

void reconfigurableRegions::printDetails() {
  std::cout << "\t\t\t'Reconfigurable Regions'\n";
}

// never should happen unless you're trying to bring down a module (read done, now copy)
void reconfigurableRegions::attemptModule(unsigned module_index) { // attempt an insert; use repl alg to decide approach

  // legacy quit-out
  throw "This should be dead code.  How did you get here?";
}


bool reconfigurableRegions::isModulePresent(unsigned region_id, unsigned module_id) {

  std::map<unsigned, module*>::iterator region_it = module_reference_table_.find(region_id);

  // Is there a module record and does it match the target module ID?
  if (
    region_it != module_reference_table_.end() &&
    region_it->second->getModuleId() == module_id
  ) {
    return true;
  }
  return false;
}

void reconfigurableRegions::insertModule(module* new_module) {
  //module_table_.push_back(new_module);

  // std::map<unsigned, module*>::iterator region_it
  //   = module_reference_table_.find(new_module->getRegionId());

  module_reference_table_[new_module->getRegionId()] = new_module;
}

// Operational Methods (unique) //

module* reconfigurableRegions::getModuleFromRegion(unsigned region_id) {
  return module_reference_table_[region_id];
}

bool reconfigurableRegions::isRegionPopulated(unsigned region_id) {
  module* target_module = getModuleFromRegion(region_id);

  return target_module->getSpeed() != 0;
}
