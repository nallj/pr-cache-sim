#ifndef NALLJ_DRACHMA_SCHEDULING_ALG
#define NALLJ_DRACHMA_SCHEDULING_ALG

#include <memory> // make_unique, unique_ptr

#include <cppJsonGraph/graph.hpp>
#include <cppJsonGraph/graphNode.hpp>

class schedulingAlg {
protected:
  std::shared_ptr<nallj::graph> task_graph_;

  schedulingAlg(const char* name, std::shared_ptr<nallj::graph> task_graph);

public:
  const char* name_;

  virtual ~schedulingAlg();

  virtual std::string peekCurrentTaskId() = 0;
  virtual nallj::graphNode& peekCurrentTask() = 0;
  virtual void blockCurrentTask() = 0;
  virtual void prepareNextTask() = 0;
};

#endif
