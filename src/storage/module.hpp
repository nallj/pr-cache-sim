#ifndef _MODULE_H_
#define _MODULE_H_

#include <string>
#include <boost/multiprecision/gmp.hpp>
#include <math.h>
#include <iostream>

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

public:
  module();
  module(unsigned id, unsigned bitstream_width, double speed);
  module(unsigned region_id, unsigned module_id, unsigned bitstream_width, double speed);

  moduleState getModuleState();
  unsigned getRegionId();
  unsigned getModuleId();
  void reassignId(unsigned new_id);
  void alterSpeed(double new_speed);
  unsigned getSize();
  double getSpeed();
  unsigned getCurrentTimestep();
  unsigned getRemainingExecutionLatency();
  void beginTransfer();
  void beginExecution(double sim_speed, unsigned execution_time);
  bool step();
  bool isRunning();
  void spillGuts();
};

#endif
