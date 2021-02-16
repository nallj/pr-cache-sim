#ifndef NALLJ_DRACHMA_ICAP
#define NALLJ_DRACHMA_ICAP

#include <deque>
#include <functional>
#include <iostream>
#include <memory> // make_shared, shared_ptr
#include <queue>
#include <stdexcept> // runtime_error
#include <unordered_map> // unordered_map
#include <vector>

#include "../globals.hpp"
#include "../signalContext.hpp"
#include "../traceToken.hpp"
#include "../storage/memoryLevel.hpp"
#include "../storage/reconfigurableRegions.hpp"
#include "../types.hpp"

class signalContext;

enum icapState {
  ICAP_INIT,
  ICAP_IDLE,
  ICAP_ACK, // need to rename
  ICAP_WAIT,
  // ICAP_LOCK_PRR,
  ICAP_TRANSFER
};

class icap {
  double icap_speed_;
  unsigned icap_bus_width_;
  bitstreamSizeMap_t region_width_;
  double sim_speed_;

  // priority queue for active programming requests
  std::priority_queue<traceToken*, std::vector<traceToken*>, std::greater<traceToken*>> pending_requests_;

  unsigned* prc_mc_;
  unsigned memory_counter_;
  unsigned target_rr_;

  bool transferring_; // When deasserted, target_rr_ is not valid.
  unsigned long transfer_latency_;
  unsigned long internal_timestep_;

  // internal signals
  unsigned stall_count_;
  icapState current_state_, next_state_;

  // memory pointers
  std::vector<storageUnit*>* memory_hierarchy_;
  reconfigurableRegions* prrs_;

  std::deque<bool>* prr_executing_;
  std::deque<bool> prr_ctrl_req_;
  bool* prr_ctrl_ack_;

  bool prc_ack_;
  bool* prc_req_;
  // traceToken** current_trace_ptr_;
  // traceToken* current_trace_;

  std::shared_ptr<moduleSpec> prc_scheduled_bs_;

  //nallj::graphNode* current_task_;
  std::shared_ptr<moduleSpec> transfer_bs_;
  bool transfer_bs_valid_;

  unsigned calculateTransferLatency(unsigned region_id);

public:
  icap(double speed, unsigned bus_width);
  ~icap();

  void setRegionWidths(bitstreamSizeMap_t region_width);
  void setSimulationSpeed(double sim_speed);

  void connect(
    std::vector<storageUnit*>* memory_hierarchy,
    reconfigurableRegions* prrs,
    signalContext& signals
  );

  void step();

  //bool transferBeginning() { return transferring_ && internal_timestep_ == 0; }
  bool transferFinished();

  // ICAP_MC
  // getter function used by the signal context to retrieve signals
  unsigned* accessNumberSignal(icapSignal signal);

  // ICAP_PRC_ACK, ICAP_TRANSFER
  // getter function used by the signal context to retrieve signals
  bool* accessSignal(icapSignal signal);

  // ICAP_PRR_REQ
  // getter function used by the signal context to retrieve signal buses
  std::deque<bool>* accessSignalBus(icapSignal signal);

  // traceToken** accessTrace();
  std::shared_ptr<moduleSpec> accessTransferBitstream() const;
};

#endif
