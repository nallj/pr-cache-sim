#ifndef NALLJ_DRACHMA_FCFS_SCHEDULER
#define NALLJ_DRACHMA_FCFS_SCHEDULER

#include <deque> // deque
#include <memory> // shared_ptr
#include <string> // string
#include <unordered_map> // unordered_map
#include <utility> // pair
#include <vector> // vector

#include <cppJsonGraph/graphNode.hpp>

#include "taskScheduler.hpp"

using graph = nallj::graph;
using graphNode = nallj::graphNode;
using nodePair = std::pair<std::string, graphNode>;

class fcfsScheduler : public taskScheduler {

  std::deque<nodePair> task_queue_;
  // Used for keeping track of tasks already added to the queue.
  std::unordered_map<std::string, void*> task_queue_lookup_;

  std::vector<std::shared_ptr<graphNode>> blocked_tasks_;
  std::unordered_map<std::string, std::shared_ptr<graphNode>> blocked_tasks_map_;

  // internal pointers n stuff

  void initialize();
  graphNode& peekCurrentTask();
  std::string peekCurrentTaskId();
  void removeCurrentTask();

public:
  fcfsScheduler(std::shared_ptr<nallj::graph> task_graph);

  graphNode getCurrentTask();
  // TODO: Who is the audience for this task?
  void blockCurrentTask();
  void prepareNextTask();
  // void unblock(key)
};

#endif
