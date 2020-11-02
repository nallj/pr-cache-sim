#ifndef NALLJ_DRACHMA_PRR_SCHEDULING
#define NALLJ_DRACHMA_PRR_SCHEDULING

#include <memory> // make_unique, unique_ptr

// #include <cppJsonGraph/graph.hpp>
// #include <cppJsonGraph/graphNode.hpp>

// I need to know tha current status of the PRR controllers, and the relationship between tasks and bitstreams; the latter
// of which is not yet expressed in the .app file. However, it can be done by having "rrX module Y tasks: task_t0, ..."
class rrScheduling {
protected:
  //prr_signlgs prrSIgnals

  rrScheduling(const char* name, rr_signlgs prrSIgnals);

public:
  const char* name_;

  // virtual nallj::graphNode& giveMeTheNextTask() = 0;
  virtual std::string getPrrChoiceForTask(some task object) = 0;
};

#endif
