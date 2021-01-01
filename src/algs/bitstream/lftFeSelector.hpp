#ifndef NALLJ_DRACHMA_LFT_FE_SELECTOR
#define NALLJ_DRACHMA_LFT_FE_SELECTOR

#include <memory> // make_unique, unique_ptr
#include <unordered_map> // unordered_map

// #include <cppJsonGraph/graph.hpp>
#include <cppJsonGraph/graphNode.hpp>

#include "../../signalContext.hpp"
#include "../../specs/bitstream.hpp"
#include "../../types.hpp"
#include "../../userError.hpp"
#include "bitstreamSelector.hpp"

class signalContext;

class lftFeSelector : public bitstreamSelector {
  const taskRrLookupMap_t& bs_capabilites_;
  signalContext& signals_;

public:
  lftFeSelector(const taskRrLookupMap_t& bs_capabilites, signalContext& signals);

  // nallj::graphNode& giveMeTheNextTask() = 0;
  bitstream getBitstreamForTask(const nallj::graphNode& task) const;
};

#endif
