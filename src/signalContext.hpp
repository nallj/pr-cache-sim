#ifndef NALLJ_DRACHMA_SIGNAL_CONTEXT
#define NALLJ_DRACHMA_SIGNAL_CONTEXT

#include <deque>
#include <iostream>
#include <memory> // shared_ptr, unique_ptr
#include <vector>

#include <cppJsonGraph/graphNode.hpp>

#include "globals.hpp"
#include "components/prc.hpp"
#include "components/prrLevelController.hpp"
#include "components/icap.hpp"
// #include "traceToken.hpp"
#include "specs/moduleSpec.hpp"

class icap;
class prc;

class signalContext {

  // PRR controller context
  unsigned prr_ctrl_count_;
  std::deque<unsigned*> rrc_bitstream_;
  std::deque<bool*> prr_executing_;
  std::deque<bool*> prr_prc_ack_;
  std::deque<bool*> prr_icap_ack_;

  std::deque<unsigned> stored_rrc_bitstream_;
  std::deque<bool> stored_prr_executing_;
  bool stored_prr_prc_ack_;
  bool stored_prr_icap_ack_;
  
  // PRC context
  unsigned* prc_mc_;
  bool* prc_icap_req_;
  std::deque<bool>* prc_enqueue_prr_;
  std::deque<bool>* prc_start_prr_;
  // traceToken** prc_current_trace_;
  nallj::nodePtr prc_current_task_;
  std::shared_ptr<moduleSpec> prc_scheduled_bs_;

  unsigned stored_prc_mc_;
  bool stored_prc_icap_req_;
  std::deque<bool> stored_prc_enqueue_prr_;
  std::deque<bool> stored_prc_start_prr_;
  // traceToken stored_prc_current_trace_;
  nallj::graphNode stored_prc_current_task_;
  moduleSpec stored_prc_scheduled_bs_;
  
  // ICAP context
  unsigned* icap_mc_;
  bool* icap_prc_ack_;
  std::deque<bool>* icap_prr_ctrl_req_;
  bool* icap_transfer_;
  // traceToken** icap_current_trace_;
  unsigned* icap_target_rr_;
  std::shared_ptr<moduleSpec> icap_transfer_bs_;

  unsigned stored_icap_mc_;
  bool stored_icap_prc_ack_;
  std::deque<bool> stored_icap_prr_ctrl_req_;
  bool stored_icap_transfer_;
  // traceToken stored_icap_current_trace_;
  unsigned stored_icap_target_rr_;

public:
  signalContext(
    std::vector<prrLevelController*>* prr_ctrls,
    std::shared_ptr<prc> prc,
    std::shared_ptr<icap> icap
  );

  void refreshContext(bool printRefreshProgress = false, bool beVeryDetailed = false);

  // PRR_PRC_ACK, PRR_ICAP_ACK
  bool* accessContextSignal(prrCtrlSignal signal);

  // PRR_EXE
  std::deque<bool>* accessContextSignalBus(prrCtrlSignal signal);

  // RRC_BITSTREAM
  std::deque<unsigned>* accessContextNumberSignal(prrCtrlSignal signal);

  // PRC_MC
  unsigned* accessContextCounterSignal(prcSignal signal);

  // PRC_ICAP_REQ
  bool* accessContextSignal(prcSignal signal);

  // PRC_START_PRR, PRC_ENQUEUE_PRR
  bool* accessContextSignal(prcSignal signal, unsigned bus_bit);

  // ICAP_MC
  unsigned* accessContextNumberSignal(icapSignal signal);

  // ICAP_PRC_ACK, ICAP_TRANSFER_PRR
  bool* accessContextSignal(icapSignal signal);

  // ICAP_PRR_REQ
  bool* accessContextSignal(icapSignal signal, unsigned bus_bit);

  // traceToken** accessContextCurrentTrace(bool fromPrcElseIcap);
  std::shared_ptr<moduleSpec> accessContextIcapTransferBitstream() const;
  std::shared_ptr<moduleSpec> accessContextPrcScheduledBitstream() const;
  nallj::nodePtr accessContextCurrentTask() const;
};

#endif
