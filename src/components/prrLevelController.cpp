#include "prrLevelController.h"

static const unsigned prrStateStalls[] = {
  0, // PRR_INIT
  0, // PRR_IDLE
  0, // PRR_WAIT
  0  // PRR_TRANSFER
};

/* PUBLIC */

prrLevelController::prrLevelController(
  unsigned prr_id,
  reconfigurableRegions* memory_hierarchy_top,
  std::vector<storageUnit*>* memory_hierarchy,
  double sim_speed
) :
  prr_id_(prr_id),
  prr_(memory_hierarchy_top),
  memory_hierarchy_(memory_hierarchy),
  sim_speed_(sim_speed),
  next_state_(PRR_INIT),
  prr_executing_(false),
  icap_ack_(false),
  prc_ack_(false) { }


// prrLevelController::~prrLevelController() { };

void prrLevelController::connect(
  bool *prc_start,
  bool *prc_enqueue,
  unsigned *icap_mc,
  bool *icap_req,
  bool *icap_trans,
  traceToken** prc_current_trace_ptr,
  traceToken** icap_current_trace_ptr
) {

  prc_current_trace_ptr_ = prc_current_trace_ptr;
  prc_start_ = prc_start;
  prc_enqueue_ = prc_enqueue;

  icap_mc_ = icap_mc;
  icap_current_trace_ptr_ = icap_current_trace_ptr;
  icap_req_ = icap_req;
  icap_trans_ = icap_trans;
}

void prrLevelController::step() {

  std::cout << "PRR_CTRL[" << prr_id_ << "]: ";

  // stalling helps the simulator more closely behave as hardware would.
  //if (stall_count_ != 0) {
  //  stall_count_--;
  //  std::cout << "stalling (" << stall_count_ << " left)...\n";

  //} else {

    // latch current state and associated stall count.
    current_state_ = next_state_;
    //stall_count_ = prcStateStalls[current_state_];

    // the PRC is attempting to ENQUEUE its' current trace in the PRR CTRL's action queue
    if (*prc_enqueue_ && !prc_ack_) {
      std::cout << "(trying to ENQUEUE PRC trace, ";
      action_queue_.push(*prc_current_trace_ptr_);
      std::cout << "SUCCESS) ";

      prc_ack_ = true;

    } else if (prc_ack_ && !*prc_enqueue_) {
      std::cout << "(turning off PRC_ACK now) ";
      prc_ack_ = false;
    }

    // if the PRR CTRL is ACKing because of a previous request but
    // the request is complete, the PRR CTRL should stop ACKing.
    if (icap_ack_ && !*icap_req_) {
      std::cout << "(turning off ICAP_ACK now) ";
      icap_ack_ = false;
    }

    module* prr_content = prr_->getModuleFromRegion(prr_id_);

    std::ostringstream prr_status;
    prr_status << "\tPRR[" << prr_content->getRegionId() << "]: ";

    //if (prr_executing_) {// && !prr_->getModuleFromRegion(prr_id_)->isRunning()) {

      //bool module_finished = prr_content->step();

      switch (prr_content->getModuleState()) {
        case VACANT:
          prr_status << "is VACANT.";
          break;

        case IDLE:
          prr_status << "contains Module# " << prr_content->getModuleId()
                  << " and is IDLE.";
          break;

        case TRANSFER:
          prr_status << "is receiving a TRANSFER from the ICAP.";
          break;

        case EXEC:

          if (prr_content->step()) {
            prr_status << "just finished EXECUTING Module# " << prr_content->getModuleId()
                    << " task.";
            prr_executing_ = false;

          } else {
            prr_status << "is EXECUTING Module# " << prr_content->getModuleId() << " ("
                    << prr_content->getRemainingExecutionLatency()
                    << " sim cycles remaining).";
          }
          break;

        default:
          prr_status << "<ERROR> PRR is in an UNKNOWN STATE!\n";
      }
      prr_status << std::endl;

      // if the region's resident module is finishes running, deassert PRR_EXE
      //if (module_finished)
      //  prr_executing_ = false;
    //}


    // unsigned region_id, module_id;
    // if (*prc_current_trace_ptr_) {
    //   region_id = (*prc_current_trace_ptr_)->getRegionId();
    //   module_id = (*prc_current_trace_ptr_)->getModuleId();
    // }

    switch (current_state_) {
      case PRR_INIT:
        std::cout << "passing INIT stage\n";
        next_state_ = PRR_IDLE;
        break;

      case PRR_IDLE:
        std::cout << "passing IDLE stage - ";

        // answering enqueued requests takes greatest priority.
        if (action_queue_.size() != 0) {
          std::cout << "there is a pending request from the PRC ";

          if (prr_content->getModuleState() == IDLE) {
            traceToken* enqueued_trace = action_queue_.front();

            std::cout << "and the module of concern is ready to go, begin EXECUTING ";

            prr_content->beginExecution(sim_speed_, enqueued_trace->getExectionTime());

            prr_executing_ = true;
            action_queue_.pop();

          } else {
            std::cout << "but the module of concern is currently busy.\n";
          }

        // always handle requests from the PRC to begin execution first; the target
        // module is already present and all that is needed is a simple run request.
        // ASSUMPTION: PRR is programmed with desired module.
        } else if (*prc_start_) {

          std::cout << "received start request from PRC; ";

          // the PRC can only request a module to execute if it is actually
          // residing in it's corresponding region and the module is IDLE
          if (prr_content->getModuleState() == VACANT) {
            std::cout << "ERROR: PRC is requesting PRR CTRL to begin execution of module "
                  << "within PRR[" << prr_id_ << "] but the region is VACANT!\n";
    
          } else if (prr_content->getModuleState() == IDLE) {
            std::cout << "beginning EXECUTION ";

            // Save pointer to the current trace pointed at when the PRC made its request.
            prc_current_trace_ = *prc_current_trace_ptr_;

            // TODO: Isn't this always the same value as assigned at line 127
            //    e.g. region_id = (*prc_current_trace_ptr_)->getRegionId();
            auto region_id = prc_current_trace_->getRegionId();
            prr_content = prr_->getModuleFromRegion(region_id);

            unsigned exe_duration = prc_current_trace_->getExectionTime();
            prr_content->beginExecution(sim_speed_, exe_duration);

            prr_executing_ = true;

          } else {
            std::cout << "the module cannot begin executing since it is currently occupied";

            if (prr_content->getModuleState() == EXEC) {
              std::cout << " EXECUTING (should have enqueued instead of PRR_START)\n";
            } else if (prr_content->getModuleState() == TRANSFER) {
              std::cout << " TRANSFERRING (should have been an ICAP_REQ)\n";
            }
          }


        // before accepting a transfer request from the ICAP, always take on an enqueued
        // request from the PRC; the target module is already there, it just needs to
        // finish a previous run of the same module.
        //} else if (*prc_enqueue_) {
        //  std::cout << "received enqueue request from ICAP.\n";

          // save pointer to the current trace pointed at when the PRC made its' request
        //  prc_current_trace_ = *prc_current_trace_ptr_;



        // only accept a transfer requests from the ICAP if there are no other requests
        } else if (*icap_req_) {
          std::cout << "received transfer request from ICAP.\n";

          // save pointer to the current trace pointed at when the ICAP made its' request
          icap_current_trace_ = *icap_current_trace_ptr_;

          auto region_id = icap_current_trace_->getRegionId();
          auto module_id = icap_current_trace_->getModuleId();

          //std::cout << "\tIs region# " << region_id << " populated?  ";

          //if (prr_->isRegionPopulated(region_id)) {
            //std::cout << "yes it is. ";

            //module* region_module_content = prr_->getModuleFromRegion(region_id);
            module* region_contents = prr_->getModuleFromRegion(region_id);

            if (region_contents->getModuleId() == module_id) {
              std::cout << "\tThe module in target region PRR[" << prr_id_
                    << "] is already the correct module (how did this happen?)\n";
              throw "This state should never occur.";
              /*if (region_module_content->isRunning()) {
                // wait until done then execute
              } else {
                // begin running
              }*/

            }

            if (region_contents->isRunning()) {
              std::cout << "\tThe module in target region PRR[" << prr_id_
                    << "] is currently busy EXECUTING and can't take on the request\n";
            } else {
              // begin transfer
              icap_ack_ = true;
              prr_content->beginTransfer();

              next_state_ = PRR_TRANSFER;
            }

          //} else {
            //std::cout << "no its not. ";
            // begin

            //icap_ack_ = true;
            //next_state_ = PRR_TRANSFER;
          //}

          //icap_ack_ = true;
          // if (target PRR is idle)
            // next_state_ = ICAP_TRANSFER;
          // else (target PRR executing)
            // next_state_ = ICAP_WAIT;
        } else {
          std::cout << "idling.\n";
        }

        //std::cout << std::endl;
        break;

      case PRR_WAIT:
        std::cout << "passing WAIT stage ";

        // if (target PRR is idle)
          // next_state_ = ICAP_TRANSFER;

        break;

      //case PRR_ENQUEUE:
      //  std::cout << "passing PRR ENQUEUE stage\n";
      //  break;

      case PRR_TRANSFER:
        std::cout << "passing PRR TRANSFER stage";

        if (!*icap_trans_) {
          //prr_->
          std::cout << " - and the transfer is complete, beginning EXE ";

          auto region_id = icap_current_trace_->getRegionId();
          auto module_id = icap_current_trace_->getModuleId();

          // place target module into its' corresponding PRR

          // Get the memory level pointed at by the ICAP's memory counter.
          auto memory_level = dynamic_cast<memoryLevel*>(memory_hierarchy_->at(*icap_mc_));

          auto transferred_module = memory_level->getModule(region_id, module_id);

          //transferred_module->spillGuts();
          prr_->insertModule(transferred_module);

          auto exe_duration = icap_current_trace_->getExectionTime();
          transferred_module->beginExecution(sim_speed_, exe_duration);
          //region_contents->beginExecution(sim_speed_, exe_duration);

          prr_executing_ = true;
          next_state_ = PRR_IDLE;

        } else {
          std::cout << std::endl;
        }

        break;

      default:
        std::cout << "ERROR: PRR Controller #" << prr_id_ << " is in an UNKNOWN STATE!\n";
        throw "Should be unreachable.";
    }
  //}

  //prc_counter_++;
  std::cout << prr_status.str();
}

// PRR_EXE, PRR_PRC_ACK, PRR_ICAP_ACK
bool* prrLevelController::accessSignal(prrCtrlSignal signal) {
  switch (signal) {
    case PRR_EXE:
      return &prr_executing_;

    case PRR_PRC_ACK:
      return &prc_ack_;

    case PRR_ICAP_ACK:
      return &icap_ack_;

    default:
      return nullptr;
  }
}
