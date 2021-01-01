#ifndef NALLJ_DRACHMA_ICAP
#define NALLJ_DRACHMA_ICAP

#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <deque>
#include <unordered_map> // unordered_map

#include "../globals.hpp"
#include "../signalContext.hpp"
#include "../traceToken.hpp"
#include "../storage/memoryLevel.hpp"
#include "../storage/reconfigurableRegions.hpp"
#include "../types.hpp"

class signalContext;

enum icapState { ICAP_INIT, ICAP_IDLE, ICAP_WAIT, ICAP_LOCK_PRR, ICAP_TRANSFER };

class icap {
  double icap_speed_;
  unsigned icap_bus_width_;
  bitstreamSizeMap_t region_width_;
  double sim_speed_;

  // priority queue for active programming requests
  std::priority_queue<traceToken*, std::vector<traceToken*>, std::greater<traceToken*>> pending_requests_;

  unsigned* prc_mc_;
  unsigned memory_counter_;

  bool transferring_;
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
  bool *prc_req_;
  traceToken** current_trace_ptr_;
  traceToken* current_trace_;

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
  unsigned* accessCounterSignal(icapSignal signal);

  // ICAP_PRC_ACK, ICAP_TRANSFER
  // getter function used by the signal context to retrieve signals
  bool* accessSignal(icapSignal signal);

  // ICAP_PRR_REQ
  // getter function used by the signal context to retrieve signal buses
  std::deque<bool>* accessSignalBus(icapSignal signal);

  traceToken** accessTrace();
};

#endif
