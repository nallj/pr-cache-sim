#include "signalContext.hpp"

/* PUBLIC */

signalContext::signalContext(std::vector<prrLevelController*>* prr_ctrls, prc* prc, icap* icap) :
  prr_ctrl_count_(prr_ctrls->size()),
  stored_prr_prc_ack_(false),
  stored_prr_icap_ack_(false),
  stored_prc_start_prr_(false),
  stored_prc_enqueue_prr_(false),
  stored_prc_icap_req_(false),
  stored_icap_prc_ack_(false),
  stored_icap_transfer_(false),
  icap_current_trace_(nullptr) {

  // all stored variables are initialized to false (logical zero). individual stored variables
  // are declared in the class initialization list whilst the stored bus signal are declared below.
  for (unsigned i = 0; i < prr_ctrl_count_; i++) {
    stored_prr_executing_.push_back(false);
    stored_prc_enqueue_prr_.push_back(false);
    stored_prc_start_prr_.push_back(false);
    stored_icap_prr_ctrl_req_.push_back(false);
  }


  // now associate all signal context pointers to the corresponding signal or signal bus.

  // PRR controller signals
  for (unsigned i = 0; i < prr_ctrl_count_; i++) {
    prr_executing_.push_back(prr_ctrls->at(i)->accessSignal(PRR_EXE));
    prr_prc_ack_.push_back(prr_ctrls->at(i)->accessSignal(PRR_PRC_ACK));
    prr_icap_ack_.push_back(prr_ctrls->at(i)->accessSignal(PRR_ICAP_ACK));
  }

  // PRC output pointers
  prc_current_trace_ = prc->accessTrace();
  prc_mc_ = prc->accessCounterSignal(PRC_MC);
  prc_start_prr_ = prc->accessSignalBus(PRC_START_PRR);
  prc_enqueue_prr_ = prc->accessSignalBus(PRC_ENQUEUE_PRR);
  prc_icap_req_ = prc->accessSignal(PRC_ICAP_REQ);

  // ICAP output pointers
  icap_current_trace_ = icap->accessTrace();
  icap_mc_ = icap->accessCounterSignal(ICAP_MC);
  icap_prc_ack_ = icap->accessSignal(ICAP_PRC_ACK);
  icap_prr_ctrl_req_ = icap->accessSignalBus(ICAP_PRR_REQ);
  icap_transfer_ = icap->accessSignal(ICAP_TRANSFER_PRR);
};


void signalContext::refreshContext(bool printRefreshProgress, bool beVeryDetailed) {

  std::cout << "\n<REFRESHING SIGNAL CONTEXT>\n";

  bool value_before_change;
  std::ostringstream status_out, status_sub;


  /*** PRC signal statuses ****************************************************/

  // PRC_CURR_TRACE ///////////////////
  if (*prc_current_trace_) {
    if (printRefreshProgress)
      std::cout << "\tPRC_LATCHED_TRACE = <" << *prc_current_trace_ << ">\n";

    stored_prc_current_trace_ = **prc_current_trace_;
  }
  /////////////////////////////////////


  // PRC_MC ///////////////////
  stored_prc_mc_ = *prc_mc_;


  // PRC_ICAP_REQ ///////////////////
  bool prc_icap_req = *prc_icap_req_;
  value_before_change = stored_prc_icap_req_;
  if (printRefreshProgress)
    std::cout << "\tPRC_ICAP.req = '" << prc_icap_req << "' ... PRC_ICAP.req was '"
          << (stored_prc_icap_req_ ? "true" : "false");

  stored_prc_icap_req_ = prc_icap_req;

  if (printRefreshProgress)
    std::cout << "' and is now '"
          << (stored_prc_icap_req_ ? "true" : "false") << "'"
          << ((value_before_change != stored_prc_icap_req_) ? "\t<-- CHANGE" : (stored_prc_icap_req_ ? "\t(ASSERTED)" : ""))
          << "\n";
  //////////////////////////////


  // PRC_START_PRR[i] //////////
  status_out << "\tPRC_PRR_CTRL[0.." << (prr_ctrl_count_ - 1) << "].start = ⟨ ";
  status_sub << produceSpaces(36, 1, prr_ctrl_count_ - 1);

  for (unsigned i = 0; i < prr_ctrl_count_; i++) {

    bool prc_start_prr = (*prc_start_prr_)[i];
    value_before_change = stored_prc_start_prr_[i];
    if (printRefreshProgress && beVeryDetailed)
      std::cout << "\tPRC_PRR_" << i << ".start = '" << prc_start_prr << "' [ref]; "
            << "PRC_PRR_" << i << ".start was '"
            << (stored_prc_start_prr_[i] ? "true" : "false");

    stored_prc_start_prr_[i] = prc_start_prr;

    status_out << stored_prc_start_prr_[i] << " ";
    status_sub << ((value_before_change != stored_prc_start_prr_[i])
            ? "* " : (stored_prc_start_prr_[i] ? "^ " : "  "));

    if (printRefreshProgress && beVeryDetailed)
      std::cout << "' and is now '"
            << (stored_prc_start_prr_[i] ? "true" : "false") << "'"
            << ((value_before_change != stored_prc_start_prr_[i]) ? "\t<-- CHANGE" : (stored_prc_start_prr_[i] ? "\t(ASSERTED)" : ""))
            << "\n";
  }

  status_out << "⟩";
  if (printRefreshProgress && !beVeryDetailed)
    std::cout << status_out.str() << "\n" << status_sub.str() << "\n";
  status_out.str("");
  status_sub.str("");
  //////////////////////////////


  // PRC_ENQUEUE_PRR[i] ////////
  status_out << "\tPRC_PRR_CTRL[0.." << (prr_ctrl_count_ - 1) << "].enqueue = ⟨ ";
  status_sub << produceSpaces(38, 1, prr_ctrl_count_ - 1);

  for (unsigned i = 0; i < prr_ctrl_count_; i++) {

    bool prc_enqueue_prr = prc_enqueue_prr_->at(i);
    value_before_change = stored_prc_enqueue_prr_[i];
    if (printRefreshProgress && beVeryDetailed)
      std::cout << "\tPRC_PRR_" << i << ".enqueue = '" << prc_enqueue_prr << "' [ref]; "
            << "PRC_PRR_" << i << ".enqueue was '"
            << (stored_prc_enqueue_prr_[i] ? "true" : "false");

    stored_prc_enqueue_prr_[i] = prc_enqueue_prr;

    status_out << stored_prc_enqueue_prr_[i] << " ";
    status_sub << ((value_before_change != stored_prc_enqueue_prr_[i])
            ? "* " : (stored_prc_enqueue_prr_[i] ? "^ " : "  "));

    if (printRefreshProgress && beVeryDetailed)
      std::cout << "' and is now '"
            << (stored_prc_enqueue_prr_[i] ? "true" : "false") << "'"
            << ((value_before_change != stored_prc_enqueue_prr_[i]) ? "\t<-- CHANGE" : (stored_prc_enqueue_prr_[i] ? "\t(ASSERTED)" : ""))
            << "\n";

  }


  status_out << "⟩";
  if (printRefreshProgress && !beVeryDetailed)
    std::cout << status_out.str() << "\n" << status_sub.str() << "\n";
  status_out.str("");
  status_sub.str("");
  //////////////////////////////

  /*** end PRC signal statuses ************************************************/





  /*** ICAP signal statuses ***************************************************/

  // ICAP_CURR_TRACE //////////////////
  if (*icap_current_trace_ != nullptr && *icap_current_trace_) {
    if (printRefreshProgress)
      std::cout << "\tICAP_LATCHED_TRACE = <" << *icap_current_trace_ << ">\n";

    stored_icap_current_trace_ = **icap_current_trace_;
  }
  /////////////////////////////////////


  // PRC_MC ///////////////////
  stored_icap_mc_ = *icap_mc_;


  // ICAP_PRC_ACK ///////////////////
  bool icap_prc_ack = *icap_prc_ack_;
  value_before_change = stored_icap_prc_ack_;
  if (printRefreshProgress)
    std::cout << "\tICAP_PRC.ack = '" << icap_prc_ack << "' ... ICAP_PRC.ack was '"
          << (stored_icap_prc_ack_ ? "true" : "false");

  stored_icap_prc_ack_ = icap_prc_ack;

  if (printRefreshProgress)
    std::cout << "' and is now '"
          << (stored_icap_prc_ack_ ? "true" : "false") << "'"
          << ((value_before_change != stored_icap_prc_ack_) ? "\t<-- CHANGE" : (stored_icap_prc_ack_ ? "\t(ASSERTED)" : ""))
          << "\n";
  //////////////////////////////


  // ICAP_TRANSFER_PRR ///////////////////
  bool icap_transfer = *icap_transfer_;
  value_before_change = stored_icap_transfer_;
  if (printRefreshProgress)
    std::cout << "\tICAP_PRR.transfer = '" << icap_transfer << "' ... ICAP_PRR.transfer was '"
          << (stored_icap_transfer_ ? "true" : "false");

  stored_icap_transfer_ = icap_transfer;

  if (printRefreshProgress)
    std::cout << "' and is now '"
          << (stored_icap_transfer_ ? "true" : "false") << "'"
          << ((value_before_change != stored_icap_transfer_) ? "\t<-- CHANGE" : (stored_icap_transfer_ ? "\t(ASSERTED)" : ""))
          << "\n";
  //////////////////////////////


  // ICAP_PRR_REQ[i] ///////////
  status_out << "\tICAP_PRR_CTRL[0.." << (prr_ctrl_count_ - 1) << "].req = ⟨ ";
  status_sub << produceSpaces(35, 1, prr_ctrl_count_ - 1);

  for (unsigned i = 0; i < prr_ctrl_count_; i++) {

    bool icap_prr_ctrl_req = icap_prr_ctrl_req_->at(i);
    value_before_change = stored_icap_prr_ctrl_req_[i];
    if (printRefreshProgress && beVeryDetailed)
      std::cout << "\tICAP.prr[" << i << "]Req = '" << icap_prr_ctrl_req << "' [ref]; "
            << "CAP.prr[" << i << "]Req was '"
            << (stored_icap_prr_ctrl_req_[i] ? "true" : "false");

    stored_icap_prr_ctrl_req_[i] = icap_prr_ctrl_req;

    status_out << stored_icap_prr_ctrl_req_[i] << " ";
    status_sub << ((value_before_change != stored_icap_prr_ctrl_req_[i])
            ? "* " : (stored_icap_prr_ctrl_req_[i] ? "^ " : "  "));

    if (printRefreshProgress && beVeryDetailed)
      std::cout << "' and is now '"
            << (stored_icap_prr_ctrl_req_[i] ? "true" : "false") << "' " // extra space for tab
            << ((value_before_change != stored_icap_prr_ctrl_req_[i]) ? "\t<-- CHANGE" : (stored_icap_prr_ctrl_req_[i] ? "\t(ASSERTED)" : ""))
            << "\n";
  }

  status_out << "⟩";
  if (printRefreshProgress && !beVeryDetailed)
    std::cout << status_out.str() << "\n" << status_sub.str() << "\n";
  status_out.str("");
  status_sub.str("");
  //////////////////////////////

  /*** end ICAP signal statuses ***********************************************/





  /*** PRR CTRLs signal statuses **********************************************/

  // PRR_PRC_ACK[i] ////////////
  bool previously_stored_prr_prc_ack_ = stored_prr_prc_ack_;
  stored_prr_prc_ack_ = false;

  status_out << "\tPRR_CTRL[0.." << (prr_ctrl_count_ - 1) << "]_PRC.ack = ⟨ ";
  status_sub << produceSpaces(34, 1, prr_ctrl_count_ - 1);

  for (unsigned i = 0; i < prr_ctrl_count_; i++) {

    value_before_change = previously_stored_prr_prc_ack_;
    if (printRefreshProgress && beVeryDetailed)
      std::cout << "\tPRR_" << i << ".prcAck = '" << *(prr_prc_ack_[i]) << "' ["
            << &(prr_prc_ack_[i]) << "];"
            << (*(prr_prc_ack_[i]) ? "\t\t\t\t\t\t\t(ASSERTING on bus)" : "")
            << "\n";

    status_out << *(prr_prc_ack_[i]) << " ";
    status_sub << (*(prr_prc_ack_[i]) ? "^ " : "  ");

    if (*(prr_prc_ack_[i]))
      stored_prr_prc_ack_ = true;
  }

  status_out << "⟩";
  if (printRefreshProgress && !beVeryDetailed)
    std::cout << status_out.str() << "\n" << status_sub.str() << "\n";
  status_out.str("");
  status_sub.str("");
  //////////////////////////////


  // PRR_ICAP_ACK_BUS //////////
  //value_before_change = previously_stored_prr_icap_ack_;
  if (printRefreshProgress)
    std::cout << "\tPRR_CTRLs_PRC.ackBus = '" << stored_prr_prc_ack_
          << "' ... PRR_CTRLs_PRC.ackBus was '"
          << (previously_stored_prr_prc_ack_ ? "true" : "false")
          << "' and is now '"
          << (stored_prr_prc_ack_ ? "true" : "false") << "'"
          << ((previously_stored_prr_prc_ack_ != stored_prr_prc_ack_) ? "\t<-- BUS CHANGE" :
              (stored_prr_prc_ack_ ? "\t(BUS ASSERTED)" : "")) << "\n";
  //////////////////////////////


  // PRR_ICAP_ACK[i] ///////////
  bool previously_stored_prr_icap_ack_ = stored_prr_icap_ack_;
  stored_prr_icap_ack_ = false;

  status_out << "\tPRR_CTRL[0.." << (prr_ctrl_count_ - 1) << "]_ICAP.ack = ⟨ ";
  status_sub << produceSpaces(36, 1, prr_ctrl_count_ - 1);

  for (unsigned i = 0; i < prr_ctrl_count_; i++) {

    value_before_change = previously_stored_prr_icap_ack_;
    if (printRefreshProgress && beVeryDetailed)
      std::cout << "\tPRR_" << i << ".icapAck = '" << *(prr_icap_ack_[i]) << "' ["
            << &(prr_icap_ack_[i]) << "];"
            << (*(prr_icap_ack_[i]) ? "\t\t\t\t\t\t\t(ASSERTING on bus)" : "")
            << "\n";

    status_out << *(prr_icap_ack_[i]) << " ";
    status_sub << (*(prr_icap_ack_[i]) ? "^ " : "  ");

    if (*(prr_icap_ack_[i]))
      stored_prr_icap_ack_ = true;
  }

  status_out << "⟩";
  if (printRefreshProgress && !beVeryDetailed)
    std::cout << status_out.str() << "\n" << status_sub.str() << "\n";
  status_out.str("");
  status_sub.str("");
  //////////////////////////////


  // PRR_ICAP_ACK_BUS //////////
  //value_before_change = previously_stored_prr_icap_ack_;
  if (printRefreshProgress)
    std::cout << "\tPRR_CTRLs_ICAP.ackBus = '" << stored_prr_icap_ack_
          << "' ... PRR_CTRLs_ICAP.ackBus was '"
          << (previously_stored_prr_icap_ack_ ? "true" : "false")
          << "' and is now '"
          << (stored_prr_icap_ack_ ? "true" : "false") << "'"
          << ((previously_stored_prr_icap_ack_ != stored_prr_icap_ack_) ? "\t<-- BUS CHANGE" :
              (stored_prr_icap_ack_ ? "\t(BUS ASSERTED)" : "")) << "\n";
  //////////////////////////////


  // PRR_EXE[i] ////////////////
  status_out << "\tPRR[0.." << (prr_ctrl_count_ - 1) << "].exe = ⟨ ";
  status_sub << produceSpaces(25, 1, prr_ctrl_count_ - 1);

  for (unsigned i = 0; i < prr_ctrl_count_; i++) {

    bool prr_exe = *(prr_executing_[i]);
    value_before_change = stored_prr_executing_[i];
    if (printRefreshProgress && beVeryDetailed)
      std::cout << "\tPRR_" << i << ".exe = '" << prr_exe << "' [" << &(prr_executing_[i])
            << "]; PRR_CTRL_" << i << ".exe was '"
            << (stored_prr_executing_[i] ? "true" : "false");

    stored_prr_executing_[i] = prr_exe;

    status_out << stored_prr_executing_[i] << " ";
    status_sub << ((value_before_change != stored_prr_executing_[i])
            ? "* " : (stored_prr_executing_[i] ? "^ " : "  "));

    if (printRefreshProgress && beVeryDetailed)
      std::cout << "' and is now '"
            << (stored_prr_executing_[i] ? "true" : "false") << "'"
            << ((value_before_change != stored_prr_executing_[i]) ? "\t<-- CHANGE" : (stored_prr_executing_[i] ? "\t(ASSERTED)" : ""))
            << "\n";
  }

  status_out << "⟩";
  if (printRefreshProgress && !beVeryDetailed)
    std::cout << status_out.str() << "\n" << status_sub.str() << "\n";
  status_out.str("");
  status_sub.str("");
  //////////////////////////////

  /*** end PRR CTRLs signal statuses ******************************************/
}

// PRR_PRC_ACK, PRR_ICAP_ACK
bool* signalContext::accessContextSignal(prrCtrlSignal signal) {
  switch (signal) {
    case PRR_PRC_ACK:
      return &stored_prr_prc_ack_;

    case PRR_ICAP_ACK:
      return &stored_prr_icap_ack_;

    default:
      return nullptr;
  }
}

// PRR_EXE
std::deque<bool>* signalContext::accessContextSignalBus(prrCtrlSignal signal) {
  switch (signal) {
    case PRR_EXE:
      return &stored_prr_executing_;

    default:
      return nullptr;
  }
}

// PRC_MC
unsigned* signalContext::accessContextCounterSignal(prcSignal signal) {
  switch (signal) {
    case PRC_MC:
      return &stored_prc_mc_;

    default:
      return nullptr;
  }
}

// PRC_ICAP_REQ
bool* signalContext::accessContextSignal(prcSignal signal) {
  switch (signal) {
    case PRC_ICAP_REQ:
      return &stored_prc_icap_req_;

    default:
      return nullptr;
  }
}

// PRC_START_PRR, PRC_ENQUEUE_PRR
bool* signalContext::accessContextSignal(prcSignal signal, unsigned bus_bit) {
  switch (signal) {
    case PRC_START_PRR:
      return &stored_prc_start_prr_[bus_bit];

    case PRC_ENQUEUE_PRR:
      return &stored_prc_enqueue_prr_[bus_bit];

    default:
      return nullptr;
  }
}


// ICAP_MC
unsigned* signalContext::accessContextCounterSignal(icapSignal signal) {
  switch (signal) {
    case ICAP_MC:
      return &stored_icap_mc_;

    default:
      return nullptr;
  }
}

// ICAP_PRC_ACK, ICAP_TRANSFER_PRR
bool* signalContext::accessContextSignal(icapSignal signal) {
  switch (signal) {
    case ICAP_PRC_ACK:
      return &stored_icap_prc_ack_;

    case ICAP_TRANSFER_PRR:
      return &stored_icap_transfer_;

    default:
      return nullptr;
  }
}

// ICAP_PRR_REQ
bool* signalContext::accessContextSignal(icapSignal signal, unsigned bus_bit) {
  switch (signal) {
    case ICAP_PRR_REQ:
      return &stored_icap_prr_ctrl_req_[bus_bit];

    default:
      return nullptr;
  }
}

// TODO: Get rid of this. Traces are superseded by task graphs.
traceToken** signalContext::accessContextCurrentTrace(bool fromPrcElseIcap) {
  return fromPrcElseIcap ? prc_current_trace_ : icap_current_trace_;
}

/* PRIVATE */

std::string signalContext::produceSpaces(
  unsigned count,
  unsigned var_len_num_count,
  unsigned variable_length_num
) {
  std::ostringstream spaces;

  if (var_len_num_count != 0) {
    if (variable_length_num < 10)
      count += var_len_num_count;
    else if (variable_length_num < 100)
      count += var_len_num_count * 2;
    else if (variable_length_num < 1000)
      count += var_len_num_count * 3;
    else if (variable_length_num < 10000)
      count += var_len_num_count * 4;
    else
      count += var_len_num_count * 5;
  }

  for (unsigned i = 0; i < count; i++) {
    spaces << " ";
  }

  return spaces.str();
}
