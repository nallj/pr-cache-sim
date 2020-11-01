#ifndef NALLJ_DRACHMA_MODULE
#define NALLJ_DRACHMA_MODULE

#include <boost/multiprecision/gmp.hpp>
#include <math.h>
#include <iostream>
#include <stdexcept> // runtime_error
#include <string>
#include <vector> // vector


#include "../globals.hpp"

class module {

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
  bool in_simulation_;
  std::vector<std::string> task_ids_;

public:
  module();
  module(unsigned id, unsigned bitstream_width, double speed);
  module(unsigned region_id, unsigned module_id, unsigned bitstream_width, double speed);

  moduleState getModuleState() const;
  unsigned getRegionId() const;
  unsigned getModuleId() const;
  void setId(unsigned id);
  void setRrId(unsigned rr_id);
  void setSpeed(double speed);
  void setTasks(std::vector<std::string> task_ids);
  unsigned getSize() const;
  double getSpeed() const;
  unsigned getCurrentTimestep() const;
  unsigned getRemainingExecutionLatency() const;
  void beginTransfer();
  void beginExecution(double sim_speed, unsigned execution_time);
  bool step();
  bool isRunning() const;
  void spillGuts();
};

#endif
