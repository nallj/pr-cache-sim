#ifndef NALLJ_DRACHMA_MODULE_SPEC
#define NALLJ_DRACHMA_MODULE_SPEC

#include <string> // string
#include <vector> // vector

struct moduleSpec {
  unsigned region_id;
  unsigned id;
  unsigned bitstream_width;
  double speed;
  std::vector<std::string> task_ids;
};

#endif
