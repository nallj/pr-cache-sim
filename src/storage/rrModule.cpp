#include "rrModule.hpp"

std::vector<std::string> getTaskTypeIds(std::vector<taskSpec> tasks) {
  std::vector<std::string> task_type_ids;

  for (const auto& task : tasks) {
    const auto task_type_id = task.type_id;
    task_type_ids.push_back(task_type_id);
  }
  return task_type_ids;
}

rrModule::rrModule() :
  region_id_(-1), module_id_(-1), bitstream_width_(0), operating_speed_(0) { }

rrModule::rrModule(unsigned region_id) :
  region_id_(region_id), module_id_(-1), bitstream_width_(0), operating_speed_(0) { }

rrModule::rrModule(unsigned id, unsigned bitstream_width, double speed) :
  current_state_(VACANT),
  region_id_(-1),
  module_id_(id),
  bitstream_width_(bitstream_width),
  operating_speed_(speed),
  internal_timestep_(0),
  executing_(false),
  in_simulation_(false) { }

rrModule::rrModule(
  unsigned region_id,
  unsigned module_id,
  unsigned bitstream_width,
  std::vector<taskSpec> tasks
  // double speed,
  // std::string task_type
) :
  current_state_(VACANT),
  region_id_(region_id),
  module_id_(module_id),
  bitstream_width_(bitstream_width),
  // TODO: Set this when this module is being executed for an intended task type.
  // operating_speed_(speed),
  task_type_ids_(getTaskTypeIds(tasks)),
  internal_timestep_(0),
  executing_(false),
  in_simulation_(false) { }


moduleState rrModule::getModuleState() const {
  return current_state_;
}

unsigned rrModule::getRegionId() const {
  return region_id_;
}

unsigned rrModule::getModuleId() const {
  return module_id_;
}

void rrModule::setId(unsigned id) {
  if (in_simulation_) {
    const auto msg = "Can't set a module's ID after simulation has begun.";
    throw std::runtime_error(msg);
  }
  module_id_ = id;
}

void rrModule::setRrId(unsigned rr_id) {
  if (in_simulation_) {
    const auto msg = "Can't set a module's RR ID after simulation has begun.";
    throw std::runtime_error(msg);
  }
  region_id_ = rr_id;
}

void rrModule::setSpeed(double speed) {
  if (in_simulation_) {
    const auto msg = "Can't set a module's speed after simulation has begun.";
    throw std::runtime_error(msg);
  }
  operating_speed_ = speed;
}

void rrModule::setTasks(std::vector<std::string> task_type_ids) {
  if (in_simulation_) {
    const auto msg = "Can't set a module's tasks after simulation has begun.";
    throw std::runtime_error(msg);
  }
  task_type_ids_ = task_type_ids;
}

unsigned rrModule::getSize() const {
  return bitstream_width_;
}

double rrModule::getSpeed() const {
  return operating_speed_;
}

unsigned rrModule::getCurrentTimestep() const {
  return internal_timestep_;
}

unsigned rrModule::getRemainingExecutionLatency() const {
  return execution_time_ - internal_timestep_;
}

void rrModule::beginTransfer() {
  current_state_ = TRANSFER;
}

void rrModule::beginExecution(double sim_speed, unsigned execution_time) {

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
bool rrModule::step() {
  in_simulation_ = true;

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


bool rrModule::isRunning() const {
  return executing_;
};

// this is just a temporary function for spelling out what this module is holding.
void rrModule::spillGuts() {

  std::cout << "I am module " << module_id_ << " with a bitstream width of "
        << bitstream_width_ << " B, is now "
        << (executing_ ? "EXECUTING" : "IDLING") << ". "
        << " Op Speed: " << operating_speed_ << " MHz, Start: "
        << execution_start_cycle_ << ", Exe Time: "
        << execution_time_ << " with a timestep of " << internal_timestep_ << "\n";
}
