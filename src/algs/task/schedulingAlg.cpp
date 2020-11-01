#include "schedulingAlg.hpp"

schedulingAlg::schedulingAlg(const char* name, std::shared_ptr<nallj::graph> task_graph) :
  name_(name), task_graph_(task_graph) {}

schedulingAlg::~schedulingAlg() {}
