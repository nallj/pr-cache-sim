#ifndef _MODULE_H_
#define _MODULE_H_

#include <string>
//#include <iomanip>      // setprecision
#include <boost/multiprecision/gmp.hpp>
//#include <gmp.h>
#include <math.h>

/////////
#include <iostream>
/////////

#include "../globals.h"


//enum moduleState { VACANT, IDLE, TRANSFER, EXEC };

class module {

	public:
		module() : region_id_(-1), module_id_(-1), bitstream_width_(0), operating_speed_(0) {}

		//module(unsigned id, unsigned bitstream_width, double speed);
		module(unsigned id, unsigned bitstream_width, double speed) :
				current_state_(VACANT), region_id_(-1), module_id_(id), bitstream_width_(bitstream_width),
				operating_speed_(speed), internal_timestep_(0), executing_(false) { }

		module(unsigned region_id, unsigned module_id, unsigned bitstream_width, double speed) :
				current_state_(VACANT), region_id_(region_id), module_id_(module_id),
				bitstream_width_(bitstream_width), operating_speed_(speed), internal_timestep_(0),
				executing_(false) { }


		moduleState getModuleState() {
			return current_state_;
		}

		unsigned getRegionId() {
			return region_id_;
		}

		//unsigned getId();
		unsigned getModuleId() {
			return module_id_;
		}

		//void reassignId(unsigned);
		void reassignId(unsigned new_id) {
			module_id_ = new_id;
		}

		//void alterSpeed(double);
		void alterSpeed(double new_speed) {
			operating_speed_ = new_speed;
		}

		//unsigned getSize();
		unsigned getSize() {
			return bitstream_width_;
		}

		//double getSpeed();
		double getSpeed() {
			return operating_speed_;
		}

		//unsigned getCurrentTimestep();
		unsigned getCurrentTimestep() {
			return internal_timestep_;
		}

		//unsigned getRemainingExecutionLatency();
		unsigned getRemainingExecutionLatency() {
			return execution_time_ - internal_timestep_;
		}

		void beginTransfer() {
			current_state_ = TRANSFER;
		}

		//void beginExecution(double sim_speed, unsigned execution_time);
 		void beginExecution(double sim_speed, unsigned execution_time) {

 			current_state_ = EXEC;
			executing_ = true;
			//execution_start_cycle_ = start_cycle;
			execution_time_ = execution_time;

			execution_latency_ = ceil((execution_time * sim_speed) / operating_speed_);

			//std::cout << "(will take " << execution_time << " * " << sim_speed << "/" << operating_speed_
			//		  << " = " << execution_latency_ << " sim cycles to complete).\n";
			std::cout << "(" << execution_latency_ << " sim cycles to complete).\n";
		}

		//bool step(double reference_speed, unsigned long long current_cycle);
		//bool step();
		bool step() {

			/*unsigned long ref = sim_speed * 100;
			unsigned long mod = operating_speed_ * 100;
			unsigned long long elapsed_cycles = current_sim_cycle - execution_start_cycle_;

			internal_timestep_ = (mod * elapsed_cycles) / ref;

			return internal_timestep_;*/

			bool isFinished = ++internal_timestep_ > execution_latency_;
			if (isFinished) {
				internal_timestep_ = 0;
				current_state_ = IDLE;
			}

			return isFinished;
		}

		/*/void reset();// { executing_ = false; };
		void reset() {
			executing_ = false;

			internal_timestep_ = 0;
			execution_time_ = 0;
			execution_latency_ = 0;
		}*/


		//void toggleExe();

		bool isRunning() { return executing_; };

		void spillGuts() {

			std::cout << "I am module " << module_id_ << " with a bitstream width of "
					  << bitstream_width_ << " B, is now "
					  << (executing_ ? "EXECUTING" : "IDLING") << ". "
					  << " Op Speed: " << operating_speed_ << " MHz, Start: "
					  << execution_start_cycle_ << ", Exe Time: "
					  << execution_time_ << " with a timestep of " << internal_timestep_ << "\n";
		}


	private:
		//unsigned long getEuclidsGcd(unsigned long, unsigned long);

		moduleState current_state_;

		unsigned region_id_;
		unsigned module_id_;

		unsigned bitstream_width_;

		bool executing_;

		double operating_speed_;

		unsigned long long execution_start_cycle_;

		unsigned long execution_time_;

		unsigned execution_latency_;

		unsigned long internal_timestep_;
};



#endif
