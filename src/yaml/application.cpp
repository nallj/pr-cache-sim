#include "application.hpp"

taskSchedulingType getTasktaskSchedulingType(const std::string& str) {
  if (str == "fcfs") {
    return FCFS;
  }
  throw userError("Invalid task scheduling algorithm specified.");
}

rrSelectionType getrrSelectionAlgPolicyType(const std::string& str) {
  if (str == "lf") {
    return LFT_FE;
  }
  throw userError("Invalid PRR scheduling algorithm specified.");
}

namespace YAML {
  Node convert<application>::encode(const application& rhs) {
    throw std::runtime_error("There's no support for encoding this file type.");
  }

  bool convert<application>::decode(const Node& node, application& rhs) {

    rhs.name = node["name"].as<std::string>();

    // ICAP
    const auto& icap_node = node["ICAP"];
    rhs.icap_speed = icap_node["speed"].as<double>();
    rhs.icap_bus_width = icap_node["width"].as<unsigned>();

    // PRC
    const auto& prc_node = node["PRC"];
    rhs.prc_speed = prc_node["speed"].as<double>();

    // Scheduling
    const auto task_scheduling_type = node["task scheduling"].as<std::string>();
    const auto rr_scheduling_alg_type = node["rr selection policy"].as<std::string>();

    // Get appropriate enum types from string.
    rhs.task_scheduling_alg = getTasktaskSchedulingType(task_scheduling_type);
    rhs.rr_scheduling_alg = getrrSelectionAlgPolicyType(rr_scheduling_alg_type);

    // SR
    rhs.static_region_speed = node["static region speed"].as<double>();
    const auto& static_regions = node["SRs"];

    srMap_t sr_map;
    for (const auto& sr_node : static_regions) {
      const auto id = sr_node["id"].as<unsigned>();
      sr_map[id] = sr_node["module count"].as<unsigned>();
    }
    rhs.static_modules = sr_map;

    // RR
    const auto& reconfig_regions = node["RRs"];

    rrSpecMap_t rr_map;
    double fastest_module_speed;

    for (const auto& rr_node : reconfig_regions) {
      const auto rr_id = rr_node["id"].as<unsigned>();
      const auto bitstream_size = rr_node["bitstream size"].as<unsigned>();

      for (const auto& module_node : rr_node["modules"]) {
        moduleSpec spec;

        // Collect spec fields.
        const auto module_id = module_node["id"].as<unsigned>();
        const auto speed = module_node["speed"].as<double>();

        spec.region_id = rr_id;
        spec.bitstream_width = bitstream_size;
        spec.id = module_id;
        spec.speed = speed;
        spec.task_type_ids = module_node["task types"].as<std::vector<std::string>>();

        // Save the spec to the RR map.
        rr_map[rr_id][module_id] = spec;

        // Keep track of fasted module.
        if (speed > fastest_module_speed) {
          fastest_module_speed = speed;
        }
      }
    }
    rhs.rr_spec_map = rr_map;
    rhs.simulator_speed = fastest_module_speed;

    return true;
  }

}
