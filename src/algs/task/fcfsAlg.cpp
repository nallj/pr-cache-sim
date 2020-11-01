#include "fcfsAlg.hpp"

fcfsAlg::fcfsAlg(std::shared_ptr<nallj::graph> task_graph) :
  schedulingAlg("First Come, First Serve (FCFS)", task_graph) {
    initialize();
  }

graphNode& fcfsAlg::peekCurrentTask() {
  return task_queue_.front().second;
}

std::string fcfsAlg::peekCurrentTaskId() {
  return task_queue_.front().first;
}

void fcfsAlg::blockCurrentTask() {
  // Get current task to the blocked task list.
  const auto task_pair = task_queue_.front();

  const auto key = task_pair.first;
  const auto& current_task = task_pair.second;

  // Create a shared pointer of the task for bookkeeping.
  const auto task_ptr = std::make_shared<graphNode>(current_task);

  // Add the pointer to the blocked tasks list (and corresponding lookup).
  blocked_tasks_.push_back(task_ptr);
  blocked_tasks_map_.emplace(key, task_ptr);

  // Remove task from queue.
  task_queue_.pop_back();
}

// TODO: Consider name change (e.g. markCurrentTaskDone).
void fcfsAlg::prepareNextTask() {
  // Get current task.
  const auto& current_task = peekCurrentTask();

  // Add successors of the current task to the queue.
  const auto child_map = current_task.getTraversableNeighborMap();
  for (const auto& entry : child_map) {
    const auto key = entry.first;
    const auto find_it = task_queue_lookup_.find(key);

    // Only add this task to the queue if it's not already present.
    if (find_it == task_queue_lookup_.end()) {
      task_queue_.push_back(entry);
    }    
  }

  // Remove task from queue.
  task_queue_.pop_back();
}

/**
 * Put all the start nodes of the task graph into the task queue.
 */
void fcfsAlg::initialize() {
  const auto start_node_map = task_graph_->getStartNodeMap();
  for (const auto entry : start_node_map) {
    // Add start node to the queue.
    task_queue_.push_back(entry);

    // Record node key for quick lookup for if node has been in the queue.
    task_queue_lookup_.emplace(entry.first, nullptr);
  }
}
