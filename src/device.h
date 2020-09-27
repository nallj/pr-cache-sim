#ifndef DEVICE
#define DEVICE

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <queue>
#include <map>

#include "fileHandler.h"
#include "application.h"
#include "signalContext.h"
#include "traceToken.h"
#include "components/icap.h"
#include "components/prc.h"
#include "components/prrLevelController.h"
#include "storage/storageUnit.h"
#include "storage/memoryLevel.h"
#include "storage/reconfigurableRegions.h"

//enum memAction { SEARCH, TRANSFER, EXECUTE, VACANT, IDLE, UNKNOWN };

class device {

  std::multimap<unsigned, std::vector<module*>> static_regions_;
  //std::map<unsigned, unsigned> prr_index_;
  std::map<unsigned, unsigned> prr_census_;
  std::vector<unsigned> prr_bitstream_sizes_;
  std::vector<storageUnit*> memory_hierarchy_;
  reconfigurableRegions memory_hierarchy_top_;
  double simulator_speed_;
  //application* simulated_application_;
  icap* icap_;
  prc* prc_;
  std::vector<traceToken*>* traces_;

  // memoryLevel& associativityToRegion(unsigned module_index);
  // memoryLevel& findInCache(unsigned module_index);
  //std::tuple<unsigned, unsigned, unsigned> parseTrace(std::string trace);
  // module* getBitstreamFromLibrary(unsigned region_id, unsigned module_id);

public:
  std::string name_, file_;

  void associateHierarchy(reconfigurableRegions memory_hierarchy);
  bool prepareApplicationResources(application* app);
  void parseTraceFile(std::vector<std::string> trace_file);
  void simulateApplication(unsigned long long int stop_ccc = -1);
};

#endif
