#ifndef NALLJ_DRACHMA_LFT_FE_SELECTOR
#define NALLJ_DRACHMA_LFT_FE_SELECTOR

#include <memory> // make_unique, unique_ptr
#include <queue> // priority_queue
#include <unordered_map> // unordered_map
#include <utility> // pair
#include <vector>

#include <cppJsonGraph/graphNode.hpp>

#include "../../signalContext.hpp"
#include "../../specs/moduleSpec.hpp"
#include "../../types.hpp"
#include "../../userError.hpp"
#include "bitstreamSelector.hpp"

class signalContext;

template<class T>
using idPair_t = std::pair<unsigned, T>;
using bsSpeedPair_t = idPair_t<double>; // { bs_id, speed }
using rrBsCountPair_t = idPair_t<size_t>; // { rr_id, bs_count }
using rrFlexabilityMap_t = idLookupMap_t<unsigned>;
using targetRr_t = std::pair<bool, unsigned>; // { isFound, rr_id }

auto max_pair_compare = []<class T>(idPair_t<T> left, idPair_t<T> right) {
  const auto left_speed = left.second;
  const auto right_speed = right.second;
  return left_speed > right_speed;
};
auto min_pair_compare = []<class T>(idPair_t<T> left, idPair_t<T> right) {
  const auto left_speed = left.second;
  const auto right_speed = right.second;
  return left_speed < right_speed;
};

using fastedSpeedMaxPq_t = std::priority_queue<
  bsSpeedPair_t,
  std::vector<bsSpeedPair_t>,
  decltype(max_pair_compare)
>;

using lfRrModuleMinPq_t = std::priority_queue<
  rrBsCountPair_t,
  std::vector<rrBsCountPair_t>,
  decltype(min_pair_compare)
>;

using bsSpeedMap_t = taskLookupMap_t<idLookupMap_t<fastedSpeedMaxPq_t>>;
using lfRrMap_t = taskLookupMap_t<lfRrModuleMinPq_t>;

class lftFeSelector : public bitstreamSelector {
  const taskRrLookupMap_t& bs_capabilites_; // TODO: Do I still need this?
  const bsSpeedMap_t bs_speed_map_;
  const rrSpecMap_t& rr_spec_map_;
  signalContext& signals_;
  const lfRrMap_t task_lft_rr_map_;

  targetRr_t getRrForTask(const nallj::graphNode& task) const;

public:
  lftFeSelector(
    const taskRrLookupMap_t& bs_capabilites,
    rrSpecMap_t& rr_spec_map,
    signalContext& signals
  );

  // nallj::graphNode& giveMeTheNextTask() = 0;
  targetBs_t getBitstreamForTask(const nallj::graphNode& task) const;
};

#endif
