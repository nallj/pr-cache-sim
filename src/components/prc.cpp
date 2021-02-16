#include "prc.hpp"

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


prc::prc(
  double prc_speed,
  taskSchedulingType scheduling_alg_type,
  rrSelectionType bs_sel_policy_type,
  rrSpecMap_t rr_spec_map
) :
  prc_speed_(prc_speed),
  next_state_(PRC_INIT),
  prc_counter_(0),
  memory_counter_(0),
  trace_counter_(0),
  stall_count_(0),
  scheduling_alg_type_(scheduling_alg_type),
  prr_ctrl_execute_(false),
  bs_sel_policy_type_(bs_sel_policy_type),
  memory_ack_(false),
  icap_req_(false),
  rr_spec_map_(rr_spec_map),
  scheduled_task_valid_(false) { }

prc::~prc() {
  delete mem_search_done_;
  delete mem_search_found_;
  // delete current_trace_;
}

void prc::connect(
  std::vector<storageUnit*>* memory_hierarchy,
  reconfigurableRegions* memory_hierarchy_top,
  std::shared_ptr<nallj::graph> task_graph,
  const taskRrLookupMap_t& bs_capabilites,
  signalContext& signals
) {
  // IN signals
  memory_hierarchy_ = memory_hierarchy;
  memory_hierarchy_top_ = memory_hierarchy_top;
  
  prr_executing_ = signals.accessContextSignalBus(PRR_EXE);
  prr_ack_ = signals.accessContextSignal(PRR_PRC_ACK);
  rr_bitstream_id_ = signals.accessContextNumberSignal(RRC_BITSTREAM);

  for (unsigned i = 0; i < prr_executing_->size(); i++) {
    prr_enqueue_.push_back(false);
    prr_start_.push_back(false);
  }

  icap_ack_ = signals.accessContextSignal(ICAP_PRC_ACK);
  icap_target_rr_ = signals.accessContextSignal(ICAP_TARGET_RR);
  icap_trans_ = signals.accessContextSignal(ICAP_TRANSFER_PRR);

  // Wire up the scheduler.
  switch (scheduling_alg_type_) { 
    case FCFS:
      scheduling_alg_ = std::make_unique<fcfsScheduler>(task_graph);
      break;
    default:
      throw std::invalid_argument("Unknown task scheduling algorithm.");
  }
  switch (bs_sel_policy_type_) {
    case LFT_FE:
      bs_sel_policy_ = std::make_unique<lftFeSelector>(bs_capabilites, rr_spec_map_, signals);
      break;
    default:
      throw std::invalid_argument("Unknown RR selection policy.");
  }
}

void prc::step() {
  std::cout << "PRC[cc" << prc_counter_ << "]: ";

  // Simulate stalls if you want states to take more than a single PRC cycle.
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

        next_state_ = PRC_TASK_SCHEDULE;
        break;

  // rrSelectionType bs_sel_policy_type_;
  // std::unique_ptr<bitstreamSelector> bs_sel_policy_;
  // taskSchedulingType scheduling_alg_type_;
  // std::unique_ptr<taskScheduler> scheduling_alg_;

      case PRC_TASK_SCHEDULE:
        scheduled_task_ = std::make_shared<nallj::graphNode>(scheduling_alg_->getCurrentTask());
        scheduled_task_valid_ = true;

        scheduling_alg_->prepareNextTask();

        next_state_ = PRC_RR_SCHEDULE;
        break;

      case PRC_RR_SCHEDULE: {
        const auto target_bs = bs_sel_policy_->getBitstreamForTask(*scheduled_task_.get());

        const auto bs_not_found = !target_bs.first;
        scheduled_bs_ = std::make_shared<moduleSpec>(target_bs.second);

        const auto rr_id = scheduled_bs_->region_id;
        const auto bs_id = scheduled_bs_->id;

        // Either the bitstream is already installed or the ICAP is installing this bitstream now.
        const auto target_bs_available = rr_bitstream_id_->at(rr_id) == bs_id;
        // const auto icap_transferring_to_rr = *icap_trans_ && *icap_target_rr_ == rr_id;
        const auto icap_transferring_to_rr = *icap_trans_ && *icap_target_rr_ == rr_id;
        const auto bs_not_immediately_available = icap_transferring_to_rr && !target_bs_available;

        // The task scheduler chose a task that can't be immediately scheduled. This may not be
        // desired; just in case, ask the scheduler to choose a task again.
        if (bs_not_found || bs_not_immediately_available) {
          next_state_ = PRC_TASK_SCHEDULE;
          break;
        }

        // If the desired bitstream is already installed in the RR then enqueue the task in the RR
        // controller. Otherwise, request that the ICAP fetch the bitstream.
        if (target_bs_available) {
          next_state_ = PRC_RR_ENQUEUE;
        } else {
          next_state_ = PRC_REQ_ICAP;
          icap_req_ = true;
        }
        break;
      }
      case PRC_REQ_ICAP:
        if (*icap_ack_) { // || *icap_deny_
          icap_req_ = false;
          next_state_ = PRC_TASK_SCHEDULE;
        }
        break;

      /*
      case PRC_LATCH:
        std::cout << "passing LATCH stage";

        // this wont be a necessary state if tc_ is the trace counter since it
        // will only ever be incremented by the PRC
        // current_trace_ = traces_->at(trace_counter_);
        const auto& task = scheduling_alg_->peekCurrentTask();
        const auto& task_id = scheduling_alg_->peekCurrentTaskId();

        std::cout << " while latching new task <" << task_id << ">\n";

        next_state_ = PRC_FIND_LOOP;
        break;

      case PRC_FIND_LOOP: {
        std::cout << "passing FIND LOOP stage - ";

        // This was removed because it doesn't make sense.
        // if (current_trace_->getRequestTime() <= prc_counter_) {
          // std::cout << "new request <" << current_trace_ << "> - ";

          auto region_id = 0; // current_trace_->getRegionId();
          auto module_id = 0; // current_trace_->getModuleId();


          // if ICAP is already transferring the needed module to its' corresponding
          // region, then enqueue the trace request in the target PRR CTRL action queue
          if (
            *icap_current_trace_ptr_ &&
            (*icap_current_trace_ptr_)->getRegionId() == region_id &&
            (*icap_current_trace_ptr_)->getModuleId() == module_id
          ) {

            std::cout << "module is being TRANSFERRED by the ICAP to its' region (ENQUEUING).\n";

            // if (*icap_current_trace_ptr_ == current_trace_) {
            //   std::cout << ">>>>>>>>>>> HEY! THE POINTERS MATCH UP!!! <<<<<\n\n";
            // }

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
      }
      case PRR_START:
        std::cout << "passing PRR START stage - ";

        // if (prr_executing_->at(current_trace_->getRegionId())) {
        //   std::cout << "the PRR_CTRL complied (halting request)\n";

        //   prr_start_[current_trace_->getRegionId()] = false;
        //   trace_counter_++;

        //   next_state_ = PRC_LATCH;

        // } else {
        //   std::cout << "the PRR CTRL has not complied yet.\n";
        // }

        break;

      case PRR_ENQUEUE:
        std::cout << "passing PRR ENQUEUE stage - ";

        // if the PRR CTRL ACKs proceed back
        if (*prr_ack_) {
          std::cout << "the PRR_CTRL ACKed the ENQUEUE (halting request)\n";

          // prr_enqueue_[current_trace_->getRegionId()] = false;
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
      */

      default:
        const char* msg = "PRC has reached an unknown state.";
        throw std::runtime_error(msg);
    }
  }

  prc_counter_++;
}


// OUT signals

// dont change this - this is good
// traceToken** prc::accessTrace() {
//   //return &current_trace_;

//   auto lol = new traceToken();
//   traceToken** rofl;
//   rofl = &lol;
//   return rofl;
// }

std::shared_ptr<moduleSpec> prc::accessScheduledBitstream() const {
  return scheduled_bs_;
}

nallj::nodePtr prc::accessScheduledTask() const {
  return scheduled_task_;
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
