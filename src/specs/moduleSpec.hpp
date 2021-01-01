#ifndef NALLJ_DRACHMA_MODULE_SPEC
#define NALLJ_DRACHMA_MODULE_SPEC

#include <string> // string
#include <vector> // vector

#include <yaml-cpp/yaml.h> // convert, Node

// TODO: Consider changing this to a bitstream instead of a module spec and insert directly into the main memory (which would likely no longer be a in the memory hierarchy list).
// That change would require task_type_ids to become scalar for one ID.
struct moduleSpec {
  unsigned region_id;
  unsigned id;
  unsigned bitstream_width;
  double speed;
  std::vector<std::string> task_type_ids;
};

#endif
