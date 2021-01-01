#ifndef NALLJ_DRACHMA_APPLICATION
#define NALLJ_DRACHMA_APPLICATION

#include <map> // multimap
#include <stdexcept> // runtime_error
#include <string> // string
#include <unordered_map> // unordered_map
#include <vector> // vector

#include "../components/prc.hpp" // taskSchedulingType, rrSelectionType
#include "../types.hpp"
#include "moduleSpec.hpp"

// using idList_t = std::vector<unsigned>;
// template<class T>
// using idLookupMap_t = std::unordered_map<unsigned, T>;
template<class T>
using rrModuleLookupMap_t = idLookupMap_t<idLookupMap_t<T>>;
// template<class T>
// using taskLookupMap_t = std::unordered_map<std::string, T>;

// using bitstreamSizeMap_t = idLookupMap_t<unsigned>;
// using taskRrLookupMap_t = idLookupMap_t<taskLookupMap_t<idList_t>>;
using rrSpecMap_t = rrModuleLookupMap_t<moduleSpec>;

// using srMap_t = std::multimap<unsigned, unsigned>;
using srMap_t = idLookupMap_t<unsigned>;
// Why the heck did I make this a multimap?

struct application {
  
  std::string file;
  unsigned icap_bus_width;
  double icap_speed;
  std::string name;
  double prc_speed;
  rrSelectionType rr_scheduling_alg;
  rrSpecMap_t rr_spec_map;
  double simulator_speed;
  srMap_t static_modules;
  double static_region_speed;
  taskSchedulingType task_scheduling_alg;

  application();
  application(
    const double icap_speed,
    const unsigned icap_bus_width,
    const double prc_speed,
    const taskSchedulingType task_scheduling_alg,
    const rrSelectionType rr_scheduling_alg,
    const double static_region_speed,
    const srMap_t& static_modules,
    const rrSpecMap_t& rr_spec_map,
    const double fastest_module_speed
  );
  
  bitstreamSizeMap_t getRrBitstreamSizes();
  taskRrLookupMap_t getRrTaskCapabilites();
  void printDetails(unsigned indents);
};

#endif
