#ifndef NALLJ_DRACHMA_BITSTREAM_SELECTOR
#define NALLJ_DRACHMA_BITSTREAM_SELECTOR

#include <memory> // make_unique, unique_ptr

// #include <cppJsonGraph/graph.hpp>
#include <cppJsonGraph/graphNode.hpp>

#include "../../specs/bitstream.hpp"

// I need to know tha current status of the PRR controllers, and the relationship between tasks and bitstreams; the latter
// of which is not yet expressed in the .app file. However, it can be done by having "rrX module Y tasks: task_t0, ..."
class bitstreamSelector {
protected:
  //prr_signlgs prrSIgnals

  bitstreamSelector(const char* name);

public:
  const char* name_;

  virtual ~bitstreamSelector();

  // virtual nallj::graphNode& giveMeTheNextTask() = 0;
  virtual bitstream getBitstreamForTask(const nallj::graphNode& task) const = 0;
};

#endif
