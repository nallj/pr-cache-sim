#ifndef NALLJ_DRACHMA_PRC
#define NALLJ_DRACHMA_PRC

#include <deque>
#include <iostream>
#include <memory> // make_shared, shared_ptr
#include <stdexcept> // invalid_argument, runtime_error
#include <utility> // move
#include <vector>

#include <cppJsonGraph/graph.hpp>
#include <cppJsonGraph/graphNode.hpp> // nodePtr

#include "../globals.hpp"
#include "../signalContext.hpp"
#include "../traceToken.hpp"
#include "../algs/bitstream/lftFeSelector.hpp"
#include "../algs/bitstream/bitstreamSelector.hpp"
#include "../algs/task/fcfsScheduler.hpp"
#include "../algs/task/taskScheduler.hpp"
#include "../specs/moduleSpec.hpp"
#include "../storage/memoryLevel.hpp"
#include "../storage/reconfigurableRegions.hpp"
#include "../types.hpp"
#include "prrLevelController.hpp"

class signalContext;

enum prcState {
  // PRC_INIT,
  // PRC_LATCH,
  // PRC_FIND_LOOP,
  // PRR_START,
  // PRR_ENQUEUE,
  // PRC_REQ_ICAP,
  // CACHE_SEARCH,
  // SEARCH_WAIT
  // PRC_ICAP_WAIT,

  PRC_INIT,
  PRC_TASK_SCHEDULE,
  PRC_RR_SCHEDULE,
  PRC_REQ_ICAP,
  PRC_RR_ENQUEUE,
};

enum taskSchedulingType {
  FCFS // First Come, First Serve
  // SJN, // Shortest Job Next
  // PRIORITY
};

enum rrSelectionType {
  LFT_FE
};

class prc {
  double prc_speed_;
  std::vector<bool> has_memory_lookup_table;
  unsigned prc_counter_, memory_counter_, trace_counter_;
  rrSpecMap_t rr_spec_map_;

  // Internal signals
  prcState current_state_, next_state_;
  unsigned stall_count_;
  // std::string scheduled_task_type_id_;
  bool scheduled_task_valid_;

  // IN signals
  bool *icap_target_rr_, *icap_trans_, *mem_search_done_, *mem_search_found_;
  std::vector<prrControllerState*> prr_ctrl_status_;
  std::deque<bool>* prr_executing_;
  std::deque<unsigned>* rr_bitstream_id_;

  std::vector<storageUnit*>* memory_hierarchy_;
  reconfigurableRegions* memory_hierarchy_top_;

  // OUT signals
  bool prr_ctrl_execute_, memory_ack_, icap_req_;
  std::deque<bool> prr_enqueue_;
  std::deque<bool> prr_start_;
  bool *icap_ack_, *prr_ack_;

  nallj::nodePtr scheduled_task_;
  std::shared_ptr<moduleSpec> scheduled_bs_;

  // IN/OUT signal
  // std::vector<traceToken*>* traces_;
  // traceToken* current_trace_;
  // std::shared_ptr<nallj::graph> task_graph_;
  // traceToken** icap_current_trace_ptr_;

  rrSelectionType bs_sel_policy_type_;
  std::unique_ptr<bitstreamSelector> bs_sel_policy_;
  taskSchedulingType scheduling_alg_type_;
  std::unique_ptr<taskScheduler> scheduling_alg_;
  taskRrLookupMap_t bs_capabilites_;

public:
  prc(
    double prc_speed,
    taskSchedulingType scheduling_alg_type,
    rrSelectionType pbs_sel_policy,
    rrSpecMap_t rr_spec_map
  );
  ~prc();

  void connect(
    std::vector<storageUnit*>* memory_hierarchy,
    reconfigurableRegions* memory_hierarchy_top,
    std::shared_ptr<nallj::graph> task_graph,
    const taskRrLookupMap_t& bs_capabilites,
    signalContext& signals
  );

  void step();


  // OUT signals

  // dont change this - this is good
  // traceToken** accessTrace();
  std::shared_ptr<moduleSpec> accessScheduledBitstream() const;
  nallj::nodePtr accessScheduledTask() const;

  // PRC_MC
  // getter function used by the signal context to retrieve signals
  unsigned* accessCounterSignal(prcSignal signal);

  // PRC_ICAP_REQ
  // getter function used by the signal context to retrieve signals
  bool* accessSignal(prcSignal signal);

  // PRC_ENQUEUE_PRR, PRC_START_PRR
  std::deque<bool>* accessSignalBus(prcSignal signal);
};

#endif
