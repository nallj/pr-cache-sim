#ifndef __ICAP_H__
#define __ICAP_H__

#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <deque>

#include "../globals.h"
#include "../traceToken.h"
#include "../storage/memoryLevel.h"
#include "../storage/reconfigurableRegions.h"


enum icapState { ICAP_INIT, ICAP_IDLE, ICAP_WAIT, ICAP_LOCK_PRR, ICAP_TRANSFER };

static const unsigned icapStateStalls[] = {
	0, // ICAP_INIT
	0, // ICAP_IDLE
	0, // ICAP_WAIT
	0, // ICAP_LOCK_PRR
	0  // ICAP_TRANSFER
};

class icap {

	public:
		icap(double speed, unsigned bus_width) :
			icap_speed_(speed), icap_bus_width_(bus_width), transferring_(false), internal_timestep_(0),
			next_state_(ICAP_INIT), prc_ack_(false), current_trace_(nullptr) {};
		~icap();


		//void requestJob(traceToken* trace) { pending_requests_.push(trace); }
		//traceToken* currentJob() { return pending_requests_.top(); }
		//bool jobsWaiting() { return !pending_requests_.empty(); }

		//bool isTransfering() { return transferring_; }
		/*void beginTransfer(double sim_speed, unsigned bitstream_width) {
			transferring_ = true;

			// transfer latency in pure system CCs
			transfer_latency_ = (bitstream_width * 8) / (icap_bus_width_ / icap_speed_) / sim_speed;
		}*/

		void setRegionWidths(std::vector<unsigned> region_width) { region_width_ = region_width; }
		void setSimulationSpeed(double sim_speed) { sim_speed_ = sim_speed; }

		void connect(std::vector<storageUnit*>* memory_hierarchy, reconfigurableRegions* prrs,
				std::deque<bool>* prr_executing, traceToken** current_trace_ptr,
				unsigned* prc_mc, bool* prc_req, bool* prr_ctrl_ack) {

			memory_hierarchy_ = memory_hierarchy;

			prrs_ = prrs;
			prr_executing_ = prr_executing;

			for (unsigned i = 0; i < prr_executing_->size(); i++)
				prr_ctrl_req_.push_back(false);

			current_trace_ptr_ = current_trace_ptr;

			prc_mc_ = prc_mc;
			prc_req_ = prc_req;
			prr_ctrl_ack_ = prr_ctrl_ack;
		}

		/*bool step() {

			/////////////////////////////
			//std::cout << "ICAP: t=" << (internal_timestep_ + 1) << " and trying to reach " << transfer_latency_;

			return ++internal_timestep_ >= transfer_latency_;
		}*/

		void step() {

			std::cout << "ICAP: ";

			// stalling helps the simulator more closely behave as hardware would.
			//if (stall_count_ != 0) {
			//	stall_count_--;
			//	std::cout << "stalling (" << stall_count_ << " left)...\n";

			//} else {

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
				if (*current_trace_ptr_)
					region_id = (*current_trace_ptr_)->getRegionId();

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

							// save pointer to the current trace pointed at when the PRC made its' request
							current_trace_ = *current_trace_ptr_;

							std::cout << " (target is region ID# " << region_id
									  << " and the module located @ ";

							if (memory_counter_ + 1 == memory_hierarchy_->size())
								std::cout << "MM).\n";
							else
								std::cout << "L" << memory_counter_ << ").\n";

							prr_ctrl_req_[region_id] = true;

							// transfer signal needs to be asserted here or when the PRR controller is moved
							// into waiting for transfer completion (after it ACKs) the PRR controller will
							// believe it is done transferring.
							transferring_ = true;

							//if (prr_executing_->at(region_id))
								next_state_ = ICAP_WAIT;
							//else
							//	next_state_ = ICAP_TRANSFER;
						} else
							std::cout << "idling.\n";

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

					/*case ICAP_LOCK_PRR:
						std::cout << "passing LOCK PRR stage\n";

						if (!*prr_ctrl_ack_) {
							transfer_latency_ = (region_width_[region_id] * 8)
												/ (icap_bus_width_ / icap_speed_) / sim_speed_;

							transferring_ = true;
							next_state_ = ICAP_TRANSFER;
						}

						break;*/

					case ICAP_TRANSFER:
						std::cout << "passing TRANSFER stage ";

						if (transfer_latency_-- == 0) {
							std::cout << "and has just FINISHED the transfer.\n";

							transferring_ = false;
							next_state_ = ICAP_IDLE;
						} else
							std::cout << "(" << transfer_latency_ << " sim cycles remain).\n";

						break;

					default:
						std::cout << "ERROR: ICAP is in an UNKNOWN STATE!\n";
				}
			//}

			//prc_counter_++;
		}

		//bool transferBeginning() { return transferring_ && internal_timestep_ == 0; }
		bool transferFinished() { return transferring_ && internal_timestep_ >= transfer_latency_; }

		/*void reset() {
			transferring_ = false;
			pending_requests_.pop();

			internal_timestep_ = 0;
			transfer_latency_ = 0;
		}*/

		// ICAP_MC
		// getter function used by the signal context to retrieve signals
		unsigned* accessCounterSignal(icapSignal signal) {
			switch (signal) {
				case ICAP_MC:
					return &memory_counter_;

				default:
					return nullptr;
			}
		}

		// ICAP_PRC_ACK, ICAP_TRANSFER
		// getter function used by the signal context to retrieve signals
		bool* accessSignal(icapSignal signal) {
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
		std::deque<bool>* accessSignalBus(icapSignal signal) {
			switch (signal) {
				case ICAP_PRR_REQ:
					return &prr_ctrl_req_;

				default:
					return nullptr;
			}
		}

		traceToken** accessTrace() { return &current_trace_; }

	private:
		unsigned calculateTransferLatency(unsigned region_id) {
			unsigned region_size = region_width_[region_id];

			//std::cout << "(region_width_B * 8b) / (icap_width / icap_speed) / sim_speed = "
			//		  << "(" << region_width_[region_id] << " * 8) / ("
			//		  << icap_bus_width_ << " / " << icap_speed_ << ") / " << sim_speed_ << "\n";

			// [(64 * 8) / (32 / 200) / 100] = 32
			//return ((region_size * 8) / (icap_bus_width_ / icap_speed_) / sim_speed_);


			//std::cout << "(region_width_B * 8b * icap_speed) / (icap_width * sim_speed) = "
			//		  << "(" << region_width_[region_id] << " * 8 * " << icap_speed_ << ") / ("
			//		  << icap_bus_width_ << " * " << sim_speed_ << ")\n";

			// [(64 * 8 * 200) / (32 * 100)] = 32
			return (region_size * 8 * icap_speed_) / (icap_bus_width_ * sim_speed_);
		}

		double icap_speed_;
		unsigned icap_bus_width_;
		std::vector<unsigned> region_width_;
		double sim_speed_;

		// priority queue for active programming requests
		std::priority_queue<traceToken*, std::vector<traceToken*>, std::greater<traceToken*>> pending_requests_;

		unsigned* prc_mc_;
		unsigned memory_counter_;

		bool transferring_;
		unsigned long transfer_latency_;
		unsigned long internal_timestep_;

		// internal signals
		icapState current_state_, next_state_;

		// memory pointers
		std::vector<storageUnit*>* memory_hierarchy_;
		reconfigurableRegions* prrs_;
		//reconfigurableRegions* prrs_;

		std::deque<bool>* prr_executing_;
		std::deque<bool> prr_ctrl_req_;
		bool* prr_ctrl_ack_;

		bool prc_ack_;
		bool *prc_req_;
		traceToken** current_trace_ptr_;
		traceToken* current_trace_;

		// IN/OUT signal
		//std::vector<traceToken*>* traces_;
};

#endif
