#include "lftFeSelector.hpp"

/* Helper Functions */

/**
 * Get a map keyed by task type ID that provides a minimum priority queue for finding the least
 * flexible RR.
 * NOTE: This was the original version I wrote. I'm saving this for when LFB is implemented.
 * @param bs_capabilites A 2D map, keyed by task type ID and then RR ID, providing a list of
 * bitstream IDs.
 *
 * @return A map keyed by task type ID that provides a minimum priority queue for finding the least
 * flexible RR
 */
lfRrMap_t __getLfRrMapOrig(const taskRrLookupMap_t& bs_capabilites) {
  lfRrMap_t lf_rr;

  // Loop over all task types.
  for (const auto& capable_rrs : bs_capabilites) {
    const auto task_type_id = capable_rrs.first;
    const auto& rr_to_modules_map = capable_rrs.second;

    lfRrModuleMinPq_t queue;

    // Loop over all the RRs capable of performing this task type.
    for (const auto rr_module : rr_to_modules_map) {
      const auto rr_id = rr_module.first;
      const auto bs_count = rr_module.second.size();

      // Enqueue in the minimum priority queue.
      queue.push({ rr_id, bs_count });
    }

    // Associate the queue to the current task type.
    lf_rr[task_type_id] = queue;
  }
  return lf_rr;
}

bool contains(const lfRrMap_t task_lf_rr_map, const std::string task_type_id) {
  return task_lf_rr_map.find(task_type_id) != task_lf_rr_map.end();
}

/**
 * Get a map for determining a module's speed based on the bitstream design.
 * @param rr_spec_map A 2D map, keyed by RR ID and then module ID, providing the module
 * specification.
 *
 * @return A 2D map, keyed by task type ID and then RR ID, that provides a maximum priority queue
 * for finding the fastest module speed.
 */
bsSpeedMap_t getBsSpeedMap(const rrSpecMap_t& rr_spec_map) {
  bsSpeedMap_t bs_speed_map;

  // Loop over the reconfigurable regions.
  for (const auto& rr_entry : rr_spec_map) {
    const auto rr_id = rr_entry.first;
    const auto& module_map = rr_entry.second;

    // Loop over the modules of the current RR.
    for (const auto module_entry : module_map) {
      const auto bs_id = module_entry.first;
      const auto module_spec = module_entry.second;

      const auto implemented_tasks = module_spec.tasks;

      // Loop over the task types the current module can execute.
      for (const auto task : implemented_tasks) {
        const auto task_type_id = task.type_id;
        const auto task_speed = task.speed;

        // Enqueue task speeds for this task type and RR.
        auto& queue = bs_speed_map[task_type_id][rr_id];
        queue.push({ bs_id, task_speed });
      }
    }
  }
  return bs_speed_map;
}

/**
 * Get the task flexabilities of all reconfigurable regions.
 * @param rr_spec_map A 2D map, keyed by RR ID and then by module ID, of all module specifications.
 *
 * @return A map of RR flexabilities, where "flexability" is measured by the amount of unique tasks
 * that the RR is capable of hosting.
 */
rrFlexabilityMap_t getRrTaskFlexabilies(const rrSpecMap_t& rr_spec_map) {
  rrFlexabilityMap_t rr_flexabilies;

  // Loop over the reconfigurable regions.
  for (const auto& rr_entry : rr_spec_map) {
    const auto rr_id = rr_entry.first;
    const auto& module_map = rr_entry.second;

    taskLookupMap_t<void*> unique_task_types;

    // Loop over the modules of the current RR.
    for (const auto module_entry : module_map) {
      const auto module_id = module_entry.first;
      const auto module_spec = module_entry.second;

      const auto implemented_tasks = module_spec.tasks;

      // Loop over the task types the current module can execute.
      for (const auto task : implemented_tasks) {
        const auto task_type_id = task.type_id;

        // Mark this task type as represented in the RR.
        unique_task_types[task_type_id] = nullptr;
      }
    }

    // Save the calculated flexability of this RR.
    rr_flexabilies[rr_id] = unique_task_types.size();
  }
  return rr_flexabilies;
}

/**
 * Get a map keyed by task type ID that provides a minimum priority queue for finding the least
 * flexible RR (in terms of unique tasks).
 * @param rr_spec_map A 2D map, keyed by RR ID and then by module ID, of all module specifications.
 * @param bs_capabilites A 2D map, keyed by task type ID and then RR ID, providing a list of
 * bitstream IDs.
 *
 * @return A map keyed by task type ID that provides a minimum priority queue for finding the least
 * flexible RR
 */
lfRrMap_t getLftRrMap(const rrSpecMap_t& rr_spec_map, const taskRrLookupMap_t& bs_capabilites) {
  const rrFlexabilityMap_t rr_flexabilies = getRrTaskFlexabilies(rr_spec_map);

  lfRrMap_t lf_rr;

  // Loop over all task types.
  for (const auto& capable_rrs : bs_capabilites) {
    const auto task_type_id = capable_rrs.first;
    const auto& rr_to_modules_map = capable_rrs.second;

    lfRrModuleMinPq_t queue;

    // Loop over all the RRs capable of performing this task type.
    for (const auto rr_modules : rr_to_modules_map) {
      const auto rr_id = rr_modules.first;
      const auto rr_flexability = rr_flexabilies.at(rr_id);

      // Enqueue in the minimum priority queue.
      queue.push({ rr_id, rr_flexability });
    }

    // Associate the queue to the current task type.
    lf_rr[task_type_id] = queue;
  }
  return lf_rr;
}

/**
 * @throws out_of_range when graph node doesn't have "type" in its metadata.
 */
std::string getTaskTypeId(const nallj::graphNode& task) {
  return task.getMetadata().at("type");
}

bool taskInLookupMap(const taskLookupMap_t<idList_t>& map, const std::string task_type_id) {
  return map.find(task_type_id) != map.end();
}

/* Constructor */

lftFeSelector::lftFeSelector(
  const taskRrLookupMap_t& bs_capabilites,
  rrSpecMap_t& rr_spec_map,
  signalContext& signals
) :
  bitstreamSelector("Least Flexible, Task (RR) + Fastest Execution (bitstream)"),
  bs_capabilites_(bs_capabilites),
  bs_speed_map_(getBsSpeedMap(rr_spec_map)),
  rr_spec_map_(rr_spec_map),
  signals_(signals),
  task_lft_rr_map_(getLftRrMap(rr_spec_map, bs_capabilites)) {}

/* Methods */

/**
 * Get a bitsream to perform the given task.
 * @param graphNode& task
 *
 * @return A pair { wasFound, moduleSpec }.
 */
targetBs_t lftFeSelector::getBitstreamForTask(const nallj::graphNode& task) const {
  const auto rr_choice = getRrForTask(task);

  // A bitstream can't be chosen if an RR wasn't chosen.
  const auto rr_not_found = !rr_choice.first;
  if (rr_not_found) {
    return { false, moduleSpec() };
  }

  const auto task_type_id = getTaskTypeId(task);
  const auto rr_id = rr_choice.second;

  const auto max_speed_pq = bs_speed_map_.at(task_type_id).at(rr_id);
  // while (!max_speed_pq.empty()) {
  //   const auto bs_id = max_speed_pq.top().first;

  //   // Use this RR if it's not busy.
  //   const auto rr_is_not_busy = !rr_exe_bus->at(rr_id);
  //   if (rr_is_not_busy) {
  //     return { true, rr_id };
  //   }

  //   max_speed_pq.pop();
  // }
  const auto bs_id = max_speed_pq.top().first;

  // TODO: Get the bitstream. Do I really need the whole thing?
  const auto& bitstream = rr_spec_map_.at(rr_id).at(bs_id);

  return { true, bitstream };
}

/**
 * Get an RR to perform the given task.
 * @param graphNode& task
 *
 * @return A pair { wasFound, rr_id }.
 */
targetRr_t lftFeSelector::getRrForTask(const nallj::graphNode& task) const {
  const auto task_type_id = getTaskTypeId(task);

  if (!contains(task_lft_rr_map_, task_type_id)) {
    throw userError("Task encountered without bitstream able to handle it.");
  }

  const auto rr_exe_bus = signals_.accessContextSignalBus(PRR_EXE);
  // Least flexible RR priority queue for this task type.
  auto lf_rr_pq = task_lft_rr_map_.find(task_type_id)->second;

  while (!lf_rr_pq.empty()) {
    const auto rr_id = lf_rr_pq.top().first;

    // Use this RR if it's not busy.
    const auto rr_is_not_busy = !rr_exe_bus->at(rr_id);
    if (rr_is_not_busy) {
      return { true, rr_id };
    }

    lf_rr_pq.pop();
  }

  // No suitable RR was found.
  return { false, 0 };
}
