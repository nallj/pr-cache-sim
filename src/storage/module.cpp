/*#ifndef _MODULE_
#define _MODULE_

#include "module.h"


/* PUBLIC */
/*	module::module(unsigned id, unsigned bitstream_width, double speed) :
			current_state_(VACANT), module_id_(id), bitstream_width_(bitstream_width),
			operating_speed_(speed), internal_timestep_(0), executing_(false) { }


	unsigned module::getId() {
		return module_id_;
	}

	void module::reassignId(unsigned new_id) {
		module_id_ = new_id;
	}

	void module::alterSpeed(double new_speed) {
		operating_speed_ = new_speed;
	}

	unsigned module::getSize() {
		return bitstream_width_;
	}

	double module::getSpeed() {
		return operating_speed_;
	}

	unsigned module::getCurrentTimestep() {
		return internal_timestep_;
	}

	unsigned module::getRemainingExecutionLatency() {
		return execution_time_ - internal_timestep_;
	}


	void module::beginExecution(double sim_speed, unsigned execution_time) {
		executing_ = true;
		//execution_start_cycle_ = start_cycle;
		execution_time_ = execution_time;

		execution_latency_ = ceil((execution_time * sim_speed) / operating_speed_);

		std::cout << "Gonna do (" << execution_time << " * " << sim_speed << ") / " << operating_speed_<< " = " << execution_latency_ << "\n";
	}

	//double sim_speed, unsigned long long current_sim_cycle
	bool module::step() {

		/*unsigned long ref = sim_speed * 100;
		unsigned long mod = operating_speed_ * 100;
		unsigned long long elapsed_cycles = current_sim_cycle - execution_start_cycle_;

		internal_timestep_ = (mod * elapsed_cycles) / ref;

		return internal_timestep_;*/

/*		return ++internal_timestep_ >= execution_latency_;
	}

	void module::reset() {
		executing_ = false;

		internal_timestep_ = 0;
		execution_time_ = 0;
		execution_latency_ = 0;
	}


/* PRIVATE*/
	/*unsigned long module::getEuclidsGcd(unsigned long a, unsigned long b) {

		if (a == b)
			return a;

		//unsigned long long area = a * b;

		while (true) {

			if (a < b) {
				unsigned squares = b / a;
				unsigned length_taken = a * squares;

				b = b - length_taken;

				if (b == 0)
					return a;

			} else {

				unsigned squares = a / b;
				unsigned length_taken = b * squares;

				a = a - length_taken;

				if (a == 0)
					return b;
			}
		}

	}*/

//#endif
