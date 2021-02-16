#include "device.hpp"

void device::assignTaskGraph(std::shared_ptr<nallj::graph> task_graph) {
  task_graph_ = task_graph;
}

void device::associateHierarchy(reconfigurableRegions& memory_hierarchy) {
  memory_hierarchy_top_ = memory_hierarchy;

  storageUnit* ptr = &memory_hierarchy;
  memory_hierarchy_.push_back(ptr);

  while ((ptr = ptr->getChild())) {
    memory_hierarchy_.push_back(ptr);
  }
}

/**
 * @param app Application the device will prepare for.
 * @return if a this device is able to facilitate the given application.
 */
bool device::prepareAppResources(std::shared_ptr<application> app) {

  // instantiate all static regions with their rrModule contents
  const auto& static_module_counts = app->static_modules;

  // iterate through each static region
  auto static_it = static_module_counts.begin();
  for (; static_it != static_module_counts.end(); static_it++) {

    const auto& region_id = static_it->first;
    std::vector<rrModule*> current_static_modules;

    // TODO: consider allowing for each static region to have its' own speed

    // iterate through each module in the target static region
    for (auto i = 0; i < static_it->second; i++) {
      current_static_modules.push_back(new rrModule(i, 0, app->simulator_speed));
		}

    static_regions_.insert(std::pair<unsigned, std::vector<rrModule*>>(region_id, current_static_modules));
  }

  const auto rr_specs = app->rr_spec_map;

  // iterate through each reconfigurable region; for each region, create a placeholder module
  unsigned required_bitstream_space = 0;

  // for each reconfigurable module, add it to the bitstream library in main memory
  auto main_memory = memory_hierarchy_.back();

  for (auto it = rr_specs.begin(); it != rr_specs.end(); it++) {

    const auto region_id = it->first;
    const auto module_specs = it->second;
    const auto bitstream_width = module_specs.begin()->second.bitstream_width;

    // Insert a placeholder module at the RR level.
    const auto new_mod = new rrModule(region_id);
    memory_hierarchy_top_.insertModule(new_mod);

    // track the amount of space required for the main memory to store all reconfigurable modules
    const auto region_module_count = module_specs.size();
    required_bitstream_space += region_module_count * bitstream_width;

    // document the amount of modules in each region for later use
    rr_census_.insert(std::pair<unsigned, unsigned>(region_id, region_module_count));

    // Instantiate the bitstream library.
    for (const auto module_entry : module_specs) {
      const auto spec = module_entry.second;
      const auto& tasks = spec.tasks;

      const auto rr_space = new rrModule(region_id, spec.id, bitstream_width, tasks);
      main_memory->insertModule(rr_space);
    }
  }

  // TODO: check each cache level and warn the user if there is not enough space for
  // certain modules; fail if a level cant even hold one module

  prr_bitstream_sizes_ = app->getRrBitstreamSizes();
  simulator_speed_ = app->simulator_speed;
  bs_capabilites_ = app->getRrTaskCapabilites();

  icap_ = std::make_shared<icap>(app->icap_speed, app->icap_bus_width);
  prc_ = std::make_shared<prc>(
    app->prc_speed,
    app->task_scheduling_alg,
    app->rr_scheduling_alg,
    app->rr_spec_map
  );

  // return true if the bitstream library can fit in the chosen main memory
  return main_memory->getSize() >= required_bitstream_space;
}

void device::simulateApplication(const unsigned long long int stop_ccc) {

  // Simulator clock cycle count
  unsigned long long int ccc = 0;
  unsigned long long int max_ccc = -1;

  // Get the PRR count.
  const auto prr_count = rr_census_.size();

  std::cout << "\nINFO: Simulator will execute at a rate of " << simulator_speed_ << " MHz.\n";

  // Create the PRR level controllers.
  std::vector<prrLevelController*> prr_level_controllers;
  for (auto i = 0; i < prr_count; i++) {
    prr_level_controllers.push_back(
      new prrLevelController(i, &memory_hierarchy_top_, &memory_hierarchy_, simulator_speed_));
	}
  // TODO: prrLevelController needs access to its' PRR and the rest of the hierarchy

  // Create a signal context which will be used to "parallelize" the simulation actions.
  auto simulation_context = signalContext(&prr_level_controllers, prc_, icap_);

  // Wire up the PRC and ICAP.
  prc_->connect(
    &memory_hierarchy_,
    &memory_hierarchy_top_,
    task_graph_,
    bs_capabilites_,
    simulation_context
  );
  icap_->connect(&memory_hierarchy_, &memory_hierarchy_top_, simulation_context);

  // Inform the ICAP of the bitstream sizes for each of the reconfigurable regions.
  icap_->setRegionWidths(prr_bitstream_sizes_);
  icap_->setSimulationSpeed(simulator_speed_);

  // Wire up the PRR controllers.
  for (unsigned i = 0; i < prr_count; i++) {
    prr_level_controllers[i]->connect(
			simulation_context.accessContextSignal(PRC_START_PRR, i),
      simulation_context.accessContextSignal(PRC_ENQUEUE_PRR, i),
      simulation_context.accessContextNumberSignal(ICAP_MC),
      simulation_context.accessContextSignal(ICAP_PRR_REQ, i),
      simulation_context.accessContextSignal(ICAP_TRANSFER_PRR),
      simulation_context.accessContextCurrentTask(),
      simulation_context.accessContextPrcScheduledBitstream(),
      simulation_context.accessContextIcapTransferBitstream()
		);
	}

  // Main simulation loop
  unsigned long *trace_pointer;
  while (ccc < stop_ccc) {

    // Readability divider
    std::cout << "\n-------------------------------------------------------------------------------\n";

    // PRC and ICAP step
    prc_->step();
    icap_->step();

    std::cout << std::endl;

    // PRR controllers step
    for (auto i = 0; i < prr_level_controllers.size(); i++) {
      prr_level_controllers[i]->step();
		}

    // Propagate all signal values through the simulation signal context.
    simulation_context.refreshContext(true);

    // Stop the simulator if the maximum simulation cycle has been reached.
    if (++ccc == max_ccc) {
      std::cout << "Warning: Reached Drachma's present operational limits. Simulation ceased.";
      break;
    }

		// TODO: remove when ready.
    // DEBUG: safety stop
    if (ccc == 1000) {
			break;
		}
  }
}
