#include "module.hpp"

module::module() :
  region_id_(-1), module_id_(-1), bitstream_width_(0), operating_speed_(0) { }

module::module(unsigned id, unsigned bitstream_width, double speed) :
  current_state_(VACANT),
  region_id_(-1),
  module_id_(id),
  bitstream_width_(bitstream_width),
  operating_speed_(speed),
  internal_timestep_(0),
  executing_(false) { }

module::module(unsigned region_id, unsigned module_id, unsigned bitstream_width, double speed) :
  current_state_(VACANT),
  region_id_(region_id),
  module_id_(module_id),
  bitstream_width_(bitstream_width),
  operating_speed_(speed),
  internal_timestep_(0),
  executing_(false) { }


moduleState module::getModuleState() {
  return current_state_;
}

unsigned module::getRegionId() {
  return region_id_;
}

//unsigned getId();
unsigned module::getModuleId() {
  return module_id_;
}

//void reassignId(unsigned);
void module::reassignId(unsigned new_id) {
  module_id_ = new_id;
}

//void alterSpeed(double);
void module::alterSpeed(double new_speed) {
  operating_speed_ = new_speed;
}

//unsigned getSize();
unsigned module::getSize() {
  return bitstream_width_;
}

//double getSpeed();
double module::getSpeed() {
  return operating_speed_;
}

//unsigned getCurrentTimestep();
unsigned module::getCurrentTimestep() {
  return internal_timestep_;
}

//unsigned getRemainingExecutionLatency();
unsigned module::getRemainingExecutionLatency() {
  return execution_time_ - internal_timestep_;
}

void module::beginTransfer() {
  current_state_ = TRANSFER;
}

//void beginExecution(double sim_speed, unsigned execution_time);
void module::beginExecution(double sim_speed, unsigned execution_time) {

  current_state_ = EXEC;
  executing_ = true;
  //execution_start_cycle_ = start_cycle;
  execution_time_ = execution_time;

  execution_latency_ = ceil((execution_time * sim_speed) / operating_speed_);

  //std::cout << "(will take " << execution_time << " * " << sim_speed << "/" << operating_speed_
  //      << " = " << execution_latency_ << " sim cycles to complete).\n";
  std::cout << "(" << execution_latency_ << " sim cycles to complete).\n";
}

//bool step(double reference_speed, unsigned long long current_cycle);
bool module::step() {

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


bool module::isRunning() {
  return executing_;
};

// this is just a temporary function for spelling out what this module is holding.
void module::spillGuts() {

  std::cout << "I am module " << module_id_ << " with a bitstream width of "
        << bitstream_width_ << " B, is now "
        << (executing_ ? "EXECUTING" : "IDLING") << ". "
        << " Op Speed: " << operating_speed_ << " MHz, Start: "
        << execution_start_cycle_ << ", Exe Time: "
        << execution_time_ << " with a timestep of " << internal_timestep_ << "\n";
}
