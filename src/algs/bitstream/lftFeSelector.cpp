#include "lftFeSelector.hpp"

bool taskInLookupMap(const std::string task_type_id, const taskLookupMap_t<idList_t>& map) {
  return map.find(task_type_id) != map.end();
}

lftFeSelector::lftFeSelector(
  const taskRrLookupMap_t& bs_capabilites,
  signalContext& signals
) :
  bitstreamSelector("Least Flexible, Task (RR) + Fastest Execution (bitstream)"),
  bs_capabilites_(bs_capabilites),
  signals_(signals) { }



// void contains(const idLookupMap_t<idList_t>& rr_capabilites, std::string task_type_id) {
//   return rr_capabilites.find(task_type_id) != rr_capabilites.end();
// }



bitstream lftFeSelector::getBitstreamForTask(const nallj::graphNode& task) const {
  const auto task_type_id = task.getMetadata().find("type")->second;
  const auto rr_exe_bus = signals_.accessContextSignalBus(PRR_EXE);

  if (!bs_capabilites_.contains(task_type_id)) {
    throw userError("Task encountered without bitstream able to handle it.");
  }

  const auto& rr_capabilites = bs_capabilites_.find(task_type_id)->second;


  std::unordered_map<unsigned, unsigned> rr_capable_bitstream_counts;

  for (const auto rr : rr_capabilites) {
    const auto rr_id = rr.first;
    const auto& module_capabilities = rr.second;

    const auto rr_busy = (*rr_exe_bus)[rr_id];
    if (rr_busy) {
      continue;
    }
    


    // RR must have bitstream that supports this task. It must also not be busy to be considered.
    // if (taskInLookupMap(task_type_id, task_capabilites) && rr_not_busy) {
    // if (rr_capabilites.contains(task_type_id) && rr_not_busy) {
    //   const auto& module_ids = task_capabilites.find(task_type_id)->second;

    //   rr_capable_bitstream_counts[rr_id] = 
    // }
  }

}


