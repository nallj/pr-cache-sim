#include "memoryLevel.hpp"

memoryLevel::memoryLevel() :
  storageUnit(0, 0, 0),
  replacement_alg_(NULL) { }

// intended for main memory (automatically LUT indexed)
memoryLevel::memoryLevel(
  unsigned unit_size,
  replAlg* repl_alg,
  unsigned short read_latency,
  unsigned short search_latency
) :
  storageUnit(unit_size, read_latency, search_latency),
  replacement_alg_(repl_alg),
  contents_lut_indexed_(true) { }

// intended for cache levels
memoryLevel::memoryLevel(
  unsigned unit_size,
  replAlg* repl_alg,
  unsigned short read_latency,
  unsigned short search_latency,
  bool lut_indexed
) :
  storageUnit(unit_size, read_latency, search_latency),
  replacement_alg_(repl_alg),
  contents_lut_indexed_(lut_indexed) { }

memoryLevel::memoryLevel(const storageUnit& su) :
  storageUnit(su) { }


// Operational Methods (inherited) //

void memoryLevel::printDetails() {

  std::cout << "\t\t\t'" << name_ << "'\n"
        << "\t\t\t\tParameters: size(" << unit_size_ << "), read latency(" << read_latency_
        << "), search latency(" << search_latency_ << ")\n";

  if (replacement_alg_) replacement_alg_->printName();
}

//void attemptModule(unsigned module_index);
// never should happen unless you're trying to bring down a module (read done, now copy)
void memoryLevel::attemptModule(unsigned module_index) { // attempt an insert; use repl alg to decide approach

  // nothing here but us chickens

}

bool memoryLevel::isModulePresent(unsigned region_id,unsigned module_id) {

  std::map<unsigned, std::map<unsigned, module*>>::iterator region_it
    = module_reference_table_.find(region_id);

  if (region_it != module_reference_table_.end()) {

    std::map<unsigned, module*>::iterator module_it
      = region_it->second.find(module_id);

    if (module_it != region_it->second.end())
      return true;
  }

  return false;
}

void memoryLevel::insertModule(module* new_module) {

  unsigned region_id = new_module->getRegionId();
  unsigned module_id = new_module->getModuleId();

  std::map<unsigned, std::map<unsigned, module*>>::iterator region_it
    = module_reference_table_.find(region_id);

  if (region_it == module_reference_table_.end())
    module_reference_table_[region_id][module_id] = new_module;

  else
    region_it->second[module_id] = new_module;
}


// Operational Methods (unique) //
bool memoryLevel::isLutIndexed() {
  return contents_lut_indexed_;
};

module* memoryLevel::getModule(unsigned region_id, unsigned module_id) {
  return module_reference_table_[region_id][module_id];
}

bool memoryLevel::hasSpace() {
  return false;
}

void memoryLevel::evictModule(unsigned table_index) {
  // do eviction stuff here
}
