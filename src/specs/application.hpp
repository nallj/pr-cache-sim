#ifndef NALLJ_DRACHMA_APPLICATION
#define NALLJ_DRACHMA_APPLICATION

#include <map> // multimap
#include <stdexcept> // runtime_error
#include <unordered_map> // unordered_map
#include <vector> // vector

#include "../components/prc.hpp" // schedulingAlgType, rrSelectionPolicyType
#include "moduleSpec.hpp"

using bitstreamSizeMap_t = std::unordered_map<unsigned, unsigned>;
using rrSpecMap_t = std::unordered_map<unsigned, std::unordered_map<unsigned, moduleSpec>>;
using srMap_t = std::multimap<unsigned, unsigned>;

struct application {
  
  std::string file;
  unsigned icap_bus_width;
  double icap_speed;
  std::string name;
  double prc_speed;
  rrSelectionPolicyType rr_scheduling_alg;
  rrSpecMap_t rr_spec_map;
  double simulator_speed;
  srMap_t static_modules;
  double static_region_speed;
  schedulingAlgType task_scheduling_alg;

  application(
    const double icap_speed,
    const unsigned icap_bus_width,
    const double prc_speed,
    const schedulingAlgType task_scheduling_alg,
    const rrSelectionPolicyType rr_scheduling_alg,
    const double static_region_speed,
    const srMap_t& static_modules,
    const rrSpecMap_t& rr_spec_map,
    const double fastest_module_speed
  );
  
  bitstreamSizeMap_t getReconfigurableRegionBitstreamSizes();
  void printDetails(unsigned indents);
};

#endif
