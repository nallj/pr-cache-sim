#include "prc.h"

static const unsigned prcStateStalls[] = {
  0, // PRC_INIT
  0, // PRC_LATCH
  0, // PRC_FIND_LOOP
  0, // PRR_START
  0, // PRR_ENQUEUE
  0, // PRC_REQ_ICAP
  0, // CACHE_SEARCH
  0  // SEARCH_WAIT
};


prc::prc(double prc_speed) :
  prc_speed_(prc_speed),
  next_state_(PRC_INIT),
  prc_counter_(0),
  memory_counter_(0),
  trace_counter_(0),
  stall_count_(0),
  prr_ctrl_execute_(false),
  memory_ack_(false),
  icap_req_(false) { }

prc::~prc() {
  delete mem_search_done_;
  delete mem_search_found_;
  delete current_trace_;
}


void prc::connect(
  std::vector<storageUnit*>* memory_hierarchy,
  reconfigurableRegions* memory_hierarchy_top,
  std::vector<traceToken*>* traces,
  std::deque<bool>* prr_executing,
  bool* prr_ack,
  bool* icap_ack,
  bool *icap_trans,
  traceToken** icap_current_trace_ptr
) {

  // IN signals
  memory_hierarchy_ = memory_hierarchy;
  memory_hierarchy_top_ = memory_hierarchy_top;

  traces_ = traces;

  prr_executing_ = prr_executing;
  prr_ack_ = prr_ack;

  for (unsigned i = 0; i < prr_executing->size(); i++) {
    prr_enqueue_.push_back(false);
    prr_start_.push_back(false);
  }

  // OUT signalsS
  icap_ack_ = icap_ack;
  icap_trans_ = icap_trans;

  icap_current_trace_ptr_ = icap_current_trace_ptr;
}

void prc::step() {

  std::cout << "PRC[cc" << prc_counter_ << "]: ";

  // stalling helps the simulator more closely behave as hardware would.
  if (stall_count_ != 0) {
    stall_count_--;
    std::cout << "stalling (" << stall_count_ << " left)...\n";

  } else {

    // latch current state and associated stall count.
    current_state_ = next_state_;
    stall_count_ = prcStateStalls[current_state_];

    switch (current_state_) {
      case PRC_INIT:
        std::cout << "passing INIT stage\n";
        //pc_ = &prc_counter_;
        //mc_ = &memory_counter_;
        //tc_ = &trace_counter_;

        next_state_ = PRC_LATCH;
        break;

      case PRC_LATCH:
        std::cout << "passing LATCH stage";

        // this wont be a necessary state if tc_ is the trace counter since it
        // will only ever be incremented by the PRC
        current_trace_ = traces_->at(trace_counter_);

        std::cout << " while capturing new trace <" << current_trace_ << ">\n";

        next_state_ = PRC_FIND_LOOP;
        break;

      case PRC_FIND_LOOP:
        std::cout << "passing FIND LOOP stage - ";

        // This was removed because it doesn't make sense.
        // if (current_trace_->getRequestTime() <= prc_counter_) {
          std::cout << "new request <" << current_trace_ << "> - ";

          auto region_id = current_trace_->getRegionId();
          auto module_id = current_trace_->getModuleId();


          // if ICAP is already transferring the needed module to its' corresponding
          // region, then enqueue the trace request in the target PRR CTRL action queue
          if (
            *icap_current_trace_ptr_ &&
            (*icap_current_trace_ptr_)->getRegionId() == region_id &&
            (*icap_current_trace_ptr_)->getModuleId() == module_id
          ) {

            std::cout << "module is being TRANSFERRED by the ICAP to its' region (ENQUEUING).\n";

            if (*icap_current_trace_ptr_ == current_trace_)
              std::cout << ">>>>>>>>>>> HEY! THE POINTERS MATCH UP!!! <<<<<\n\n";

            prr_enqueue_[region_id] = true;
            next_state_ = PRR_ENQUEUE;

          // memory counter = 0, address the PRRs if the module is present.
          } else if (memory_counter_ == 0) {

            if (memory_hierarchy_top_->isModulePresent(region_id, module_id)) {
              std::cout << "module present in region ";

              //module* target_module = memory_hierarchy_top_->getModuleFromRegion(region_id);

              // region is busy executing target module.
              //if (target_module->isRunning()) {
              if (prr_executing_->at(region_id)) {
                std::cout << "but running. Enqueue request\n";

                prr_enqueue_[region_id] = true;
                next_state_ = PRR_ENQUEUE;

              // region is idling.
              } else {
                std::cout << "and idling. Begin execution\n";

                prr_start_[region_id] = true;
                next_state_ = PRR_START;
              }

            } else {
              memory_counter_++;
              std::cout << "module not present at PRR, inc MC\n";
            }

          // memory counter > 0, addressing memory levels.
          } else {

            if (memory_counter_ + 1 == memory_hierarchy_->size()) {
              std::cout << "searching MM";
            } else {
              std::cout << "searching L" << memory_counter_;
            }

            //memoryLevel* memory_level = new memoryLevel(memory_hierarchy_->at(*mc_));
            memoryLevel* memory_level = dynamic_cast<memoryLevel*>(memory_hierarchy_->at(memory_counter_));

            // if (PRC has LUT for current memory level)
            if (memory_level->isLutIndexed()) {
              std::cout << " (is LUT indexed) ";

              // if (module is @ mem[MC] level)
              if (memory_level->isModulePresent(region_id, module_id)) {
                std::cout << "and is found, request ICAP transfer\n";
                //
                // assert icap request signal
                //
                icap_req_ = true;
                next_state_ = PRC_REQ_ICAP;

              } else {
                std::cout << "and isn't found, inc MC\n";
                memory_counter_++;
              }

            } else {
              std::cout << " (not LUT indexed) - begin memory search\n";
              next_state_ = CACHE_SEARCH;
            }
          }
        // } else {
        //   std::cout << "PRC does nothing; request not made yet ("
        //         << (current_trace_->getRequestTime() - prc_counter_ - 1) << " PRC cycles left).\n";
        // }

        break;

      case PRR_START:
        std::cout << "passing PRR START stage - ";

        if (prr_executing_->at(current_trace_->getRegionId())) {
          std::cout << "the PRR_CTRL complied (halting request)\n";

          prr_start_[current_trace_->getRegionId()] = false;
          trace_counter_++;

          next_state_ = PRC_LATCH;

        } else {
          std::cout << "the PRR CTRL has not complied yet.\n";
        }

        break;

      case PRR_ENQUEUE:
        std::cout << "passing PRR ENQUEUE stage - ";

        // if the PRR CTRL ACKs proceed back
        if (*prr_ack_) {
          std::cout << "the PRR_CTRL ACKed the ENQUEUE (halting request)\n";

          prr_enqueue_[current_trace_->getRegionId()] = false;
          trace_counter_++;

          next_state_ = PRC_LATCH;

        } else {
          std::cout << "the PRR CTRL has not ACKed\n";
        }

        break;

      case PRC_REQ_ICAP:
        std::cout << "passing REQ ICAP stage ";

        if (*icap_ack_) {
          std::cout << "and the ICAP ACKed the request (halting request)\n";

          icap_req_ = false;
          memory_counter_ = 0;
          trace_counter_++;

          next_state_ = PRC_LATCH;

        } else {
          std::cout << "and the ICAP has not ACKed\n";
        }

        break;

      case CACHE_SEARCH:
        std::cout << "passing MEM SEARCH stage\n";
        break;

      case SEARCH_WAIT:
        std::cout << "passing SEARCH WAIT stage\n";
        break;

      default:
        std::cout << "ERROR: PRC is in an UNKNOWN STATE!\n";
    }
  }

  prc_counter_++;
}


// OUT signals

// dont change this - this is good
traceToken** prc::accessTrace() {
  return &current_trace_;
}

// PRC_MC
// getter function used by the signal context to retrieve signals
unsigned* prc::accessCounterSignal(prcSignal signal) {
  switch (signal) {
    case PRC_MC:
      return &memory_counter_;

    default:
      return nullptr;
  }
}

// PRC_ICAP_REQ
// getter function used by the signal context to retrieve signals
bool* prc::accessSignal(prcSignal signal) {
  switch (signal) {
    case PRC_ICAP_REQ:
      return &icap_req_;

    default:
      return nullptr;
  }
}

// PRC_ENQUEUE_PRR, PRC_START_PRR
std::deque<bool>* prc::accessSignalBus(prcSignal signal) {
  switch (signal) {
    case PRC_ENQUEUE_PRR:
      return &prr_enqueue_;

    case PRC_START_PRR:
      return &prr_start_;

    default:
      return nullptr;
  }
}
