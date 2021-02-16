#ifndef NALLJ_DRACHMA_PRR_LEVEL_CONTROLLER
#define NALLJ_DRACHMA_PRR_LEVEL_CONTROLLER

#include <memory> // shared_ptr
#include <queue>
#include <stdexcept> // runtime_error
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <cppJsonGraph/graphNode.hpp> // nallj:: nodePtr, graphNode

#include "../globals.hpp"
#include "../specs/moduleSpec.hpp"
#include "../specs/taskSpec.hpp"
#include "../storage/reconfigurableRegions.hpp"
#include "../storage/memoryLevel.hpp"
#include "../traceToken.hpp"

enum prrControllerState { PRR_INIT, PRR_IDLE, PRR_WAIT, PRR_TRANSFER };

class prrLevelController {

  prrControllerState current_state_, next_state_;

  reconfigurableRegions* prr_;
  std::vector<storageUnit*> *memory_hierarchy_;

  unsigned prr_id_;
  unsigned bitstream_id_;

  double sim_speed_;


  // IN signals
  bool *prc_start_, *prc_enqueue_, *icap_req_, *icap_trans_;

  // traceToken** prc_current_trace_ptr_;
  // traceToken* prc_current_trace_;

  unsigned* icap_mc_;
  unsigned memory_counter_;
  // traceToken** icap_current_trace_ptr_;
  // traceToken* icap_current_trace_;

  nallj::graphNode prc_current_task_;
  moduleSpec prc_scheduled_bs_;
  moduleSpec icap_transfer_bs_;

  nallj::nodePtr prc_current_task_ptr_;
  std::shared_ptr<moduleSpec> prc_scheduled_bs_ptr_;
  std::shared_ptr<moduleSpec> icap_transfer_bs_ptr_;

  // OUT signals
  bool prr_executing_, icap_ack_, prc_ack_;

  // INTERNAL
  // std::queue<traceToken*> action_queue_;
  std::queue<nallj::graphNode> action_queue_;
  // nallj::graphNode current_task_; // do I actually need this?
  moduleSpec installed_bs_;
  std::string prev_task_type_id_;

public:
  prrLevelController(
    unsigned prr_id,
    reconfigurableRegions* memory_hierarchy_top,
    std::vector<storageUnit*>* memory_hierarchy,
    double sim_speed
  );

  ~prrLevelController();

  void connect(
    bool *prc_start,
    bool *prc_enqueue,
    unsigned *icap_mc,
    bool *icap_req,
    bool *icap_trans,
    // traceToken** prc_current_trace_ptr,
    // traceToken** icap_current_trace_ptr
    nallj::nodePtr prc_current_task,
    std::shared_ptr<moduleSpec> prc_scheduled_bs,
    std::shared_ptr<moduleSpec> icap_transfer_bs
  );

  void step();

  // RRC_BITSTREAM
  unsigned* accessNumberSignal(prrCtrlSignal signal);
  // PRR_EXE, PRR_PRC_ACK, PRR_ICAP_ACK
  bool* accessSignal(prrCtrlSignal signal);
};

#endif
