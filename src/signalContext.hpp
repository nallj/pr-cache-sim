#ifndef SIGNAL_CONTEXT
#define SIGNAL_CONTEXT

#include <iostream>
#include <vector>
#include <deque>

#include "globals.hpp"
#include "components/prc.hpp"
#include "components/icap.hpp"
#include "traceToken.hpp"

class signalContext {

  // PRR controller context
  unsigned prr_ctrl_count_;
  std::deque<bool> stored_prr_executing_;
  std::deque<bool*> prr_executing_;
  bool stored_prr_prc_ack_;
  std::deque<bool*> prr_prc_ack_;
  bool stored_prr_icap_ack_;
  std::deque<bool*> prr_icap_ack_;

  unsigned stored_prc_mc_;
  unsigned* prc_mc_;
  bool stored_prc_icap_req_;
  bool* prc_icap_req_;
  std::deque<bool> stored_prc_enqueue_prr_;
  std::deque<bool>* prc_enqueue_prr_;
  std::deque<bool> stored_prc_start_prr_;
  std::deque<bool>* prc_start_prr_;

  traceToken stored_prc_current_trace_;
  traceToken** prc_current_trace_;

  unsigned stored_icap_mc_;
  unsigned* icap_mc_;
  bool stored_icap_prc_ack_;
  bool* icap_prc_ack_;
  std::deque<bool>* icap_prr_ctrl_req_;
  std::deque<bool> stored_icap_prr_ctrl_req_;
  bool* icap_transfer_;
  bool stored_icap_transfer_;

  traceToken stored_icap_current_trace_;
  traceToken** icap_current_trace_;

  std::string produceSpaces(
    unsigned count,
    unsigned var_len_num_count = 0,
    unsigned variable_length_num = 0
  );

public:
  signalContext(std::vector<prrLevelController*>* prr_ctrls, prc* prc, icap* icap);

  void refreshContext(bool printRefreshProgress = false, bool beVeryDetailed = false);

  // PRR_PRC_ACK, PRR_ICAP_ACK
  bool* accessContextSignal(prrCtrlSignal signal);

  // PRR_EXE
  std::deque<bool>* accessContextSignalBus(prrCtrlSignal signal);

  // PRC_MC
  unsigned* accessContextCounterSignal(prcSignal signal);

  // PRC_ICAP_REQ
  bool* accessContextSignal(prcSignal signal);

  // PRC_START_PRR, PRC_ENQUEUE_PRR
  bool* accessContextSignal(prcSignal signal, unsigned bus_bit);

  // ICAP_MC
  unsigned* accessContextCounterSignal(icapSignal signal);

  // ICAP_PRC_ACK, ICAP_TRANSFER_PRR
  bool* accessContextSignal(icapSignal signal);

  // ICAP_PRR_REQ
  bool* accessContextSignal(icapSignal signal, unsigned bus_bit);

  traceToken** accessContextCurrentTrace(bool fromPrcElseIcap);
};

#endif
