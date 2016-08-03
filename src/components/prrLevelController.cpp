//#ifndef _PRR_LEVEL_CONTROLLER_
//#define _PRR_LEVEL_CONTROLLER_

//#include "prrLevelController.h"


// inputs
	// current clock cycle
	// reset [not necessary?]
	// new module ready? (application scheduler)
	// is module present? module status? (prr storageUnit)

// outputs
	// push EXEC actions (prr action queue)
	// pass on SEARCH (next lowest level's action queue, aka search queue)


// internal
	// trace pointer++ (counter)

/* PUBLIC */
	//applicationScheduler& as, , application_scheduler_(&as)
	/*prrLevelController::prrLevelController(unsigned long long& ccc, storageUnit*& prrs,
											std::vector<std::queue< std::pair<unsigned, unsigned> >>& praq,
											std::queue<unsigned>& nsq, std::pair<unsigned, bool>* pc) :
			current_clock_cycle_(&ccc), prr_level_(prrs), prr_action_queue_(&praq),
			next_search_queue_(&nsq), prr_contents_(pc), current_state_(PRR_INIT), next_state_(PRR_INIT) {}

	void prrLevelController::reset() {
		trace_pointer_ = 0;
	}

	/*void prrLevelController::advance() {

		//std::cout << "Size of vectors: " << prr_action_queue_->size() << " and " << next_search_queue_->size() << "\n";
		//printf("cc: %llu", *current_clock_cycle_);
		//std::cout << ", prrs: " << prr_level_->getSize() << "\n";

		//pending_view_ << "<<< CC Count: " << *current_clock_cycle_ << " / Traces Accessed: " << trace_pointer_;

		application_scheduler_->targetTrace(trace_pointer_);

		//printf("%u == %llu", application_scheduler_->getRequestTime(), *current_clock_cycle_);

		if (application_scheduler_->getRequestTime() == (*current_clock_cycle_)) {

			//pending_view_ << " / NEXT REQUEST: " << trace_file_[trace_pointer_] << " >>>";

			unsigned requested_module_id = application_scheduler_->getModuleId();
			unsigned short target_prr = requested_module_id % prr_action_queue_->size();

			std::cout << "Module Id " << requested_module_id << " from prr count " << prr_action_queue_->size() << " = " << target_prr << "\n";

			if (prr_level_->isModulePresent(requested_module_id)) {
				//std::cout << "Module " << requested_module_id << " is present!\n";

				if (prr_contents_[0].second) { // EXEC
					(*prr_action_queue_)[target_prr].push(std::make_pair(trace_pointer_, 0)); // trace id, waiting level

					//std::queue< std::pair<unsigned, unsigned> > something = (*prr_action_queue_)[target_prr];

				} else { // IDLE
					prr_contents_[0].second = true;
					// set latency??? might be another controller's job
				}


			} else {
				//std::cout << "Module " << requested_module_id << " is NOT present!\n";
				next_search_queue_->push(trace_pointer_);
			}

			trace_pointer_++;
		} else {
			//pending_view_ << " >>>";
		}
	}

	//std::ostream prrLevelController::retrieveView() {
	//	std::ostream output_stream = pending_view_;
		//pending_stream_ = "";

	//	return output_stream;
	//}*/


/* PRIVATE */

//#endif
