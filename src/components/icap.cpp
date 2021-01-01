#include "icap.hpp"

static const unsigned icapStateStalls[] = {
  0, // ICAP_INIT
  0, // ICAP_IDLE
  0, // ICAP_WAIT
  0, // ICAP_LOCK_PRR
  0  // ICAP_TRANSFER
};

/* PUBLIC */

icap::icap(double speed, unsigned bus_width) :
  icap_speed_(speed),
  icap_bus_width_(bus_width),
  transferring_(false),
  internal_timestep_(0),
  next_state_(ICAP_INIT),
  prc_ack_(false),
  current_trace_(nullptr) { }
  
icap::~icap() { };

void icap::setRegionWidths(bitstreamSizeMap_t region_width) {
  region_width_ = region_width;
}

void icap::setSimulationSpeed(double sim_speed) {
  sim_speed_ = sim_speed;
}

void icap::connect(
  std::vector<storageUnit*>* memory_hierarchy,
  reconfigurableRegions* prrs,
  signalContext& signals
) {

  memory_hierarchy_ = memory_hierarchy;

  prrs_ = prrs;
  prr_executing_ = signals.accessContextSignalBus(PRR_EXE);

  for (unsigned i = 0; i < prr_executing_->size(); i++) {
    prr_ctrl_req_.push_back(false);
  }

  current_trace_ptr_ = signals.accessContextCurrentTrace(true);

  prc_mc_ = signals.accessContextCounterSignal(PRC_MC);
  prc_req_ = signals.accessContextSignal(PRC_ICAP_REQ);
  prr_ctrl_ack_ = signals.accessContextSignal(PRR_ICAP_ACK);
}

void icap::step() {

  std::cout << "ICAP: ";

  // stalling helps the simulator more closely behave as hardware would.
  if (stall_count_ != 0) {
    stall_count_--;
    std::cout << "stalling (" << stall_count_ << " left)...\n";

  } else {

    // latch current state and associated stall count.
    current_state_ = next_state_;
    //stall_count_ = prcStateStalls[current_state_];

    // if the ICAP is ACKing because of a previous request but
    // the request is complete, the ICAP should stop ACKing.
    if (prc_ack_ && !*prc_req_) {
      std::cout << "(turning off PRC_ACK now) ";
      prc_ack_ = false;
    }


    unsigned region_id;
    if (*current_trace_ptr_) {
      region_id = (*current_trace_ptr_)->getRegionId();
    }

    switch (current_state_) {
      case ICAP_INIT:
        std::cout << "passing INIT stage\n";
        next_state_ = ICAP_IDLE;
        break;

      case ICAP_IDLE:
        std::cout << "passing IDLE stage - ";

        if (*prc_req_) {
          std::cout << "received transfer request from PRC";

          // need to wait for PRC to ack somehow. if the ICAP is significantly faster than the
          // PRC the PRC may not notice the ack signal and it will become stuck indefinitely
          prc_ack_ = true;

          // latch the memory level holding the module found by the PRC
          memory_counter_ = *prc_mc_;

          // save  pointer to the current trace pointed at when the PRC made its' request
          current_trace_ = *current_trace_ptr_;

          std::cout << " (target is region ID# " << region_id
                << " and the module located @ ";

          if (memory_counter_ + 1 == memory_hierarchy_->size()) {
            std::cout << "MM).\n";
          } else {
            std::cout << "L" << memory_counter_ << ").\n";
          }

          prr_ctrl_req_[region_id] = true;

          // transfer signal needs to be asserted here or when the PRR controller is moved
          // into waiting for transfer completion (after it ACKs) the PRR controller will
          // believe it is done transferring.
          transferring_ = true;

          //if (prr_executing_->at(region_id))
            next_state_ = ICAP_WAIT;
          //else
          //  next_state_ = ICAP_TRANSFER;
        } else {
          std::cout << "idling.\n";
        }

        break;

      case ICAP_WAIT:
        std::cout << "passing WAIT stage";

        region_id = current_trace_->getRegionId();
        std::cout << " (target is region ID# " << region_id << ")";

        //if (!prr_executing_->at(region_id)) {
        if (*prr_ctrl_ack_) {
          std::cout << " - PRR CTRL[" << region_id << "] accepts the transfer, BEGIN!\n";

          prr_ctrl_req_[region_id] = false;
          //next_state_ = ICAP_LOCK_PRR;

          transfer_latency_ = calculateTransferLatency(region_id);

          std::cout << "\tIt will take '" << transfer_latency_
                << "' simulation cycles to complete the transfer...";

          //transferring_ = true;
          next_state_ = ICAP_TRANSFER;
        }
        std::cout << "\n";

        // if (target PRR is idle)
          // next_state_ = ICAP_TRANSFER;

        break;

      case ICAP_TRANSFER:
        std::cout << "passing TRANSFER stage ";

        if (transfer_latency_-- == 0) {
          std::cout << "and has just FINISHED the transfer.\n";

          transferring_ = false;
          next_state_ = ICAP_IDLE;
        } else {
          std::cout << "(" << transfer_latency_ << " sim cycles remain).\n";
        }

        break;

      default:
        std::cout << "ERROR: ICAP is in an UNKNOWN STATE!\n";
    }
  }

}

//bool transferBeginning() { return transferring_ && internal_timestep_ == 0; }
bool icap::transferFinished() {
  return transferring_ && internal_timestep_ >= transfer_latency_;
}

// ICAP_MC
// getter function used by the signal context to retrieve signals
unsigned* icap::accessCounterSignal(icapSignal signal) {
  switch (signal) {
    case ICAP_MC:
      return &memory_counter_;

    default:
      return nullptr;
  }
}

// ICAP_PRC_ACK, ICAP_TRANSFER
// getter function used by the signal context to retrieve signals
bool* icap::accessSignal(icapSignal signal) {
  switch (signal) {
    case ICAP_PRC_ACK:
      return &prc_ack_;

    case ICAP_TRANSFER_PRR:
      return &transferring_;

    default:
      return nullptr;
  }
}

// ICAP_PRR_REQ
// getter function used by the signal context to retrieve signal buses
std::deque<bool>* icap::accessSignalBus(icapSignal signal) {
  switch (signal) {
    case ICAP_PRR_REQ:
      return &prr_ctrl_req_;

    default:
      return nullptr;
  }
}

traceToken** icap::accessTrace() {
  return &current_trace_;
}

/* PRIVATE */

unsigned icap::calculateTransferLatency(unsigned region_id) {
  unsigned region_size = region_width_[region_id];

  // [(64 * 8 * 200) / (32 * 100)] = 32
  return (region_size * 8 * icap_speed_) / (icap_bus_width_ * sim_speed_);
}
