#ifndef NALLJ_DRACHMA_DEVICE
#define NALLJ_DRACHMA_DEVICE

#include <iostream>
#include <map> // map
#include <memory> // make_shared, shared_ptr
#include <queue>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <unordered_map> // unordered_map
#include <vector>

#include <cppJsonGraph/graph.hpp>

#include "fileHandler.hpp"
#include "signalContext.hpp"
#include "traceToken.hpp"
#include "components/icap.hpp"
#include "components/prc.hpp"
#include "components/prrLevelController.hpp"
#include "specs/application.hpp"
#include "storage/storageUnit.hpp"
#include "storage/memoryLevel.hpp"
#include "storage/reconfigurableRegions.hpp"

//enum memAction { SEARCH, TRANSFER, EXECUTE, VACANT, IDLE, UNKNOWN };

class device {

  std::multimap<unsigned, std::vector<module*>> static_regions_;
  //std::map<unsigned, unsigned> prr_index_;
  std::map<unsigned, unsigned> prr_census_;
  std::unordered_map<unsigned, unsigned> prr_bitstream_sizes_;
  std::vector<storageUnit*> memory_hierarchy_;
  reconfigurableRegions memory_hierarchy_top_;
  double simulator_speed_;
  //application* simulated_application_;
  std::shared_ptr<nallj::graph> task_graph_;
  std::shared_ptr<icap> icap_;
  std::shared_ptr<prc> prc_;

  // memoryLevel& associativityToRegion(unsigned module_index);
  // memoryLevel& findInCache(unsigned module_index);
  //std::tuple<unsigned, unsigned, unsigned> parseTrace(std::string trace);
  // module* getBitstreamFromLibrary(unsigned region_id, unsigned module_id);

public:
  std::string name_, file_;

  void assignTaskGraph(std::shared_ptr<nallj::graph> task_graph);
  void associateHierarchy(reconfigurableRegions& memory_hierarchy);
  bool prepareAppResources(std::shared_ptr<application> app);
  void simulateApplication(unsigned long long int stop_ccc = -1);
};

#endif
