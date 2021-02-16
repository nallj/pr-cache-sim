#ifndef NALLJ_DRACHMA_MODULE_SPEC
#define NALLJ_DRACHMA_MODULE_SPEC

// #include <string> // string
#include <vector> // vector

// #include <yaml-cpp/yaml.h> // convert, Node

#include "taskSpec.hpp"

struct bitstream {
  unsigned region_id;
  unsigned id;
  unsigned bitstream_width;
};

// TODO: Consider changing this to a bitstream instead of a module spec and insert directly into the main memory (which would likely no longer be a in the memory hierarchy list).
// That change would require task_type_ids to become scalar for one ID.
struct moduleSpec : public bitstream {

  // double speed; // Moved to task type level.
  // std::vector<std::string> task_type_ids;
  std::vector<taskSpec> tasks;
};

// TODO: Extend bitstream like this? Or just collapse bitstream into moduleSpec and extend that instead (and rename this to "bitstream")? Actually, if my goal is to replace the rr_spec_map in the PRC, the latter may be my only option. There's also trying to cast into another 2D map with actualBitstream as the underlying type instead (but that's a lot of copying going under the hood for no particular reason).
struct actualBitstream : public bitstream {
  bool is_cached;
  unsigned cached_level;
};

#endif
