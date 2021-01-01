#include "fcfsScheduler.hpp"

fcfsScheduler::fcfsScheduler(std::shared_ptr<nallj::graph> task_graph) :
  taskScheduler("First Come, First Serve (FCFS)", task_graph) {
    initialize();
  }

graphNode& fcfsScheduler::peekCurrentTask() {
  return task_queue_.front().second;
}

std::string fcfsScheduler::peekCurrentTaskId() {
  return task_queue_.front().first;
}

// TODO: Who is the audience for this task?
void fcfsScheduler::blockCurrentTask() {
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
  removeCurrentTask();
}

/**
 * Put all the start nodes of the task graph into the task queue.
 */
void fcfsScheduler::initialize() {
  const auto start_node_map = task_graph_->getStartNodeMap();
  for (const auto entry : start_node_map) {
    // Add start node to the queue.
    task_queue_.push_back(entry);

    // Record node key for quick lookup for if node has been in the queue.
    task_queue_lookup_.emplace(entry.first, nullptr);
  }
}

// TODO: Consider name change (e.g. markCurrentTaskDone).
void fcfsScheduler::prepareNextTask() {
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
  removeCurrentTask();
}

void fcfsScheduler::removeCurrentTask() {
  task_queue_.pop_front();
}
