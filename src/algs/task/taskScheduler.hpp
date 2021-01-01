#ifndef NALLJ_DRACHMA_TASK_SCHEDULER
#define NALLJ_DRACHMA_TASK_SCHEDULER

#include <memory> // make_unique, unique_ptr

#include <cppJsonGraph/graph.hpp>
#include <cppJsonGraph/graphNode.hpp>

class taskScheduler {
protected:
  std::shared_ptr<nallj::graph> task_graph_;

  taskScheduler(const char* name, std::shared_ptr<nallj::graph> task_graph);

public:
  const char* name_;

  virtual ~taskScheduler();

  virtual std::string peekCurrentTaskId() = 0;
  virtual nallj::graphNode& peekCurrentTask() = 0;
  virtual void blockCurrentTask() = 0;
  virtual void prepareNextTask() = 0;
};

#endif
