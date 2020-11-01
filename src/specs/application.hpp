#ifndef NALLJ_DRACHMA_APPLICATION
#define NALLJ_DRACHMA_APPLICATION

#include <map> // multimap
#include <memory> // make_unique, unique_ptr
#include <stdexcept> // runtime_error
#include <unordered_map> // unordered_map
#include <utility> // move
#include <vector> // vector

#include "../components/icap.hpp"
#include "../components/prc.hpp"
#include "moduleSpec.hpp"

using srMap_t = std::multimap<unsigned, unsigned>*;
using rrSpecMap_t = std::unordered_map<unsigned, std::unordered_map<unsigned, moduleSpec>>;

class application {
  std::unique_ptr<icap> icap_;
  std::unique_ptr<prc> prc_;

  double simulator_speed_;
  double static_region_speed_;

  srMap_t static_modules_;
  rrSpecMap_t rr_spec_map_;

  rrSpecMap_t::iterator getTargetRrIt(unsigned rr_id);

public:
  std::string name_;
  std::string file_;

  application();
  application(
    std::unique_ptr<icap>& icap,
    std::unique_ptr<prc>& prc,
    double static_region_speed,
    srMap_t static_modules,
    std::unordered_map<unsigned, std::unordered_map<unsigned, moduleSpec>> rr_spec_map,
    double fastest_module_speed
  );
  ~application();


  void printApplicationDetails();

  icap* getIcap();
  prc* getPrc();

  double getSimulatorSpeed();
  unsigned getStaticRegionCount();
  double getStaticRegionSpeed();
  srMap_t getStaticModules();
  unsigned getReconfigurableRegionCount();
  unsigned getReconfigurableRegionBitstreamSize(unsigned region_id);
  std::unordered_map<unsigned, unsigned> getReconfigurableRegionBitstreamSizes();
  rrSpecMap_t& getReconfigurableRegions();
  double getReconfigurableRegionSpeed(unsigned region_id, unsigned module_id);

  void printDetails(unsigned indents);
};

#endif
