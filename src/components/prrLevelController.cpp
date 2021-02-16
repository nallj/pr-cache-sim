#include "prrLevelController.hpp"

static const unsigned prrStateStalls[] = {
  0, // PRR_INIT
  0, // PRR_IDLE
  0, // PRR_WAIT
  0  // PRR_TRANSFER
};

/* HELPERS */

taskSpec getSpecForTaskFromBs(moduleSpec bitstream, std::string task_id) {
  for (const auto& task : bitstream.tasks) {
    if (task.type_id == task_id) {
      return task;
    }
  }
  throw std::runtime_error("Expected task spec was not present in module spec.");
}

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
  // traceToken** prc_current_trace_ptr,
  // traceToken** icap_current_trace_ptr
  nallj::nodePtr prc_current_task,
  std::shared_ptr<moduleSpec> prc_scheduled_bs,
  std::shared_ptr<moduleSpec> icap_transfer_bs
) {
  prc_scheduled_bs_ptr_ = prc_scheduled_bs;
  // prc_current_trace_ptr_ = prc_current_trace_ptr;
  prc_start_ = prc_start;
  prc_enqueue_ = prc_enqueue;

  icap_mc_ = icap_mc;
  icap_transfer_bs_ptr_ = icap_transfer_bs;
  // icap_current_trace_ptr_ = icap_current_trace_ptr;
  icap_req_ = icap_req;
  icap_trans_ = icap_trans;
}

void prrLevelController::step() {

  std::cout << "PRR_CTRL[" << prr_id_ << "]: ";

  // Simulate stalls if you want states to take more than a single RRC cycle.
  //if (stall_count_ != 0) {
  //  stall_count_--;
  //  std::cout << "stalling (" << stall_count_ << " left)...\n";

  //} else {

    // latch current state and associated stall count.
    current_state_ = next_state_;
    //stall_count_ = prcStateStalls[current_state_];

    // the PRC is attempting to ENQUEUE its' current trace in the PRR CTRL's action queue
    if (*prc_enqueue_ && !prc_ack_) {
      std::cout << "(trying to ENQUEUE PRC bitstream, ";
      // action_queue_.push(*prc_current_trace_ptr_);
      action_queue_.push(*prc_current_task_ptr_.get());
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

    auto rr_module = prr_->getModuleFromRegion(prr_id_);

    std::ostringstream rr_status;
    rr_status << "\tPRR[" << rr_module->getRegionId() << "]: ";

    //if (prr_executing_) {// && !prr_->getModuleFromRegion(prr_id_)->isRunning()) {

      //bool module_finished = rr_module->step();

      switch (rr_module->getModuleState()) {
        case VACANT:
          rr_status << "is VACANT.";
          break;

        case IDLE:
          rr_status << "contains Module# " << rr_module->getModuleId()
                  << " and is IDLE.";
          break;

        case TRANSFER:
          rr_status << "is receiving a TRANSFER from the ICAP.";
          break;

        case EXEC:
          if (rr_module->step()) {
            rr_status << "just finished EXECUTING Module# " << rr_module->getModuleId()
                    << " task.";

            prr_executing_ = false;
          } else {
            rr_status << "is EXECUTING Module# " << rr_module->getModuleId() << " ("
                    << rr_module->getRemainingExecutionLatency()
                    << " sim cycles remaining).";
          }
          break;

        default:
          rr_status << "<ERROR> PRR is in an UNKNOWN STATE!\n";
      }
      rr_status << std::endl;

      // if the region's resident module is finishes running, deassert PRR_EXE
      //if (module_finished)
      //  prr_executing_ = false;
    //}


    // unsigned region_id, module_id;
    // if (*prc_current_trace_ptr_) {
    //   region_id = (*prc_current_trace_ptr_)->getRegionId();
    //   module_id = (*prc_current_trace_ptr_)->getModuleId();
    // }
    const auto rr_id = prc_scheduled_bs_.region_id;
    const auto module_id = prc_scheduled_bs_.id;

    switch (current_state_) {
      case PRR_INIT:
        std::cout << "passing INIT stage\n";
        next_state_ = PRR_IDLE;
        break;

      case PRR_IDLE: {
        std::cout << "passing IDLE stage - ";

        // Answering enqueued requests takes greatest priority.
        if (action_queue_.size() != 0) {
          std::cout << "there is a pending request from the PRC ";

          if (rr_module->getModuleState() == IDLE) {
            // traceToken* enqueued_trace = action_queue_.front();
            const auto enqueued_task = action_queue_.front();
            action_queue_.pop();

            // TODO: Move fcfsScheduler::getTaskTypeId to a helper file and using that instead.
            prev_task_type_id_ = enqueued_task.getMetadata().at("type");
            const auto task_spec = getSpecForTaskFromBs(installed_bs_, prev_task_type_id_);
            const auto task_speed = task_spec.speed;
            const auto task_cycles = task_spec.cycles;

            std::cout << "and the module of concern is ready to go, begin EXECUTING ";

            rr_module->setSpeed(task_speed);
            rr_module->beginExecution(sim_speed_, task_cycles);

            prr_executing_ = true;

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
          if (rr_module->getModuleState() == VACANT) {
            std::cout << "ERROR: PRC is requesting PRR CTRL to begin execution of module "
                  << "within PRR[" << prr_id_ << "] but the region is VACANT!\n";
    
          } else if (rr_module->getModuleState() == IDLE) {
            std::cout << "beginning EXECUTION ";

            // Save pointer to the current trace pointed at when the PRC made its request.
            // prc_current_trace_ = *prc_current_trace_ptr_;
            prc_current_task_ = *prc_current_task_ptr_.get();
            prc_scheduled_bs_ = *prc_scheduled_bs_ptr_.get();

            // TODO: Isn't this always the same value as assigned at line 127
            //    e.g. region_id = (*prc_current_trace_ptr_)->getRegionId();
            // auto region_id = prc_current_trace_->getRegionId();
            rr_module = prr_->getModuleFromRegion(prc_scheduled_bs_.region_id);

            // TODO: Move fcfsScheduler::getTaskTypeId to a helper file and using that instead.
            // const auto task_type_id = enqueued_task.getMetadata().at("type");
            const auto task_spec = getSpecForTaskFromBs(installed_bs_, prev_task_type_id_);
            const auto task_speed = task_spec.speed;
            const auto task_cycles = task_spec.cycles;

            rr_module->setSpeed(task_speed);
            rr_module->beginExecution(sim_speed_, task_cycles);

            prr_executing_ = true;

          } else {
            std::cout << "the module cannot begin executing since it is currently occupied";

            if (rr_module->getModuleState() == EXEC) {
              std::cout << " EXECUTING (should have enqueued instead of PRR_START)\n";
            } else if (rr_module->getModuleState() == TRANSFER) {
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
          // icap_current_trace_ = *icap_current_trace_ptr_;
          icap_transfer_bs_ = *icap_transfer_bs_ptr_.get();

          const auto region_id = icap_transfer_bs_.region_id;
          const auto module_id = icap_transfer_bs_.id;

          //std::cout << "\tIs region# " << region_id << " populated?  ";

          //if (prr_->isRegionPopulated(region_id)) {
            //std::cout << "yes it is. ";

            //rrModule* region_module_content = prr_->getModuleFromRegion(region_id);
            rr_module = prr_->getModuleFromRegion(region_id);

            if (rr_module->getModuleId() == module_id) {
              std::cout << "\tThe module in target region PRR[" << prr_id_
                    << "] is already the correct module (how did this happen?)\n";
              throw "This state should never occur.";
              /*if (region_module_content->isRunning()) {
                // wait until done then execute
              } else {
                // begin running
              }*/

            }

            if (rr_module->isRunning()) {
              std::cout << "\tThe module in target region PRR[" << prr_id_
                    << "] is currently busy EXECUTING and can't take on the request\n";
            } else {
              // begin transfer
              icap_ack_ = true;
              rr_module->beginTransfer();

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
      }
      case PRR_WAIT:
        std::cout << "passing WAIT stage ";

        // if (target PRR is idle)
          // next_state_ = ICAP_TRANSFER;

        break;

      //case PRR_ENQUEUE:
      //  std::cout << "passing PRR ENQUEUE stage\n";
      //  break;

      case PRR_TRANSFER: {
        std::cout << "passing PRR TRANSFER stage";

        if (!*icap_trans_) {
          //prr_->
          std::cout << " - and the transfer is complete, beginning EXE ";

          const auto region_id = icap_transfer_bs_.region_id;
          const auto module_id = icap_transfer_bs_.id;

          // place target module into its' corresponding PRR

          // Get the memory level pointed at by the ICAP's memory counter.
          const auto memory_level = dynamic_cast<memoryLevel*>(memory_hierarchy_->at(*icap_mc_));

          const auto transferred_module = memory_level->getModule(region_id, module_id);

          //transferred_module->spillGuts();
          prr_->insertModule(transferred_module);

          // The transferred bitstream is now installed.
          installed_bs_ = *icap_transfer_bs_ptr_.get();

          // TODO: Move fcfsScheduler::getTaskTypeId to a helper file and using that instead.
          // const auto task_type_id = enqueued_task.getMetadata().at("type");
          const auto task_spec = getSpecForTaskFromBs(installed_bs_, prev_task_type_id_);
          const auto task_speed = task_spec.speed;
          const auto task_cycles = task_spec.cycles;

          // const auto exe_duration = icap_current_trace_->getExectionTime();
          transferred_module->setSpeed(task_speed);
          transferred_module->beginExecution(sim_speed_, task_cycles);
          //rr_module->beginExecution(sim_speed_, exe_duration);

          prr_executing_ = true;
          next_state_ = PRR_IDLE;

        } else {
          std::cout << std::endl;
        }

        break;
      }
      default:
        std::cout << "ERROR: PRR Controller #" << prr_id_ << " is in an UNKNOWN STATE!\n";
        throw "Should be unreachable.";
    }
  //}

  //prc_counter_++;
  std::cout << rr_status.str();
}

unsigned* prrLevelController::accessNumberSignal(prrCtrlSignal signal) {
  switch (signal) {
    case RRC_BITSTREAM:
      return &bitstream_id_;

    default:
      return nullptr;
  }
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
