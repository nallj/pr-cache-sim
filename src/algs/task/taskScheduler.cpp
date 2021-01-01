#include "taskScheduler.hpp"

taskScheduler::taskScheduler(const char* name, std::shared_ptr<nallj::graph> task_graph) :
  name_(name), task_graph_(task_graph) {}

taskScheduler::~taskScheduler() {}
