#ifndef NALLJ_DRACHMA_TASK_SPEC
#define NALLJ_DRACHMA_TASK_SPEC

#include <string> // string

struct taskSpec {
  unsigned region_id;
  unsigned module_id;
  std::string type_id;
  double speed;
  unsigned cycles;
};

#endif
