#ifndef NALLJ_DRACHMA_FCFS_ALG
#define NALLJ_DRACHMA_FCFS_ALG

#include <deque> // deque
#include <memory> // shared_ptr
#include <string> // string
#include <unordered_map> // unordered_map
#include <utility> // pair
#include <vector> // vector

#include <cppJsonGraph/graphNode.hpp>

#include "schedulingAlg.hpp"

using graph = nallj::graph;
using graphNode = nallj::graphNode;
using nodePair = std::pair<std::string, graphNode>;

class fcfsAlg : public schedulingAlg {

  std::deque<nodePair> task_queue_;
  // Used for keeping track of tasks already added to the queue.
  std::unordered_map<std::string, void*> task_queue_lookup_;

  std::vector<std::shared_ptr<graphNode>> blocked_tasks_;
  std::unordered_map<std::string, std::shared_ptr<graphNode>> blocked_tasks_map_;

  // internal pointers n stuff

  void initialize();

public:
  fcfsAlg(std::shared_ptr<nallj::graph> task_graph);

  // TODO: Do I need both peek methods?
  std::string peekCurrentTaskId();
  graphNode& peekCurrentTask();
  void blockCurrentTask();
  void prepareNextTask();
  // void unblock(key)
};

#endif
