#include "application.hpp"

application::application() { }

application::application(
  std::unique_ptr<icap>& icap,
  std::unique_ptr<prc>& prc,
  double static_region_speed,
  srMap_t static_modules,
  std::unordered_map<unsigned, std::unordered_map<unsigned, moduleSpec>> rr_spec_map,
  double fastest_module_speed
) :
  icap_(std::move(icap)), prc_(std::move(prc)),
  static_region_speed_(static_region_speed), static_modules_(static_modules),
  rr_spec_map_(rr_spec_map),
  simulator_speed_(fastest_module_speed) { }

application::~application() {
  delete static_modules_;
}


void application::printApplicationDetails() {
  std::cout << "\n\nI don't know what I got yet, man.\n\n";
}

icap* application::getIcap() {
  return icap_.get();
}
prc* application::getPrc() {
  return prc_.get();
}

double application::getSimulatorSpeed() {
  return simulator_speed_;
}

unsigned application::getStaticRegionCount() {
  return static_modules_->size();
}
double application::getStaticRegionSpeed() {
  return static_region_speed_;
}
srMap_t application::getStaticModules() {
  return static_modules_;
}

unsigned application::getReconfigurableRegionCount() {
  return rr_spec_map_.size();
}

unsigned application::getReconfigurableRegionBitstreamSize(unsigned region_id) {
  const auto target_region = getTargetRrIt(region_id);

  // Assert that the target RR has at least one entry.
  if (target_region->second.size() == 0) {
    throw std::runtime_error("Target RR has no modules to draw data from.");
  }
  return target_region->second.begin()->second.bitstream_width;
}

std::unordered_map<unsigned, unsigned> application::getReconfigurableRegionBitstreamSizes() {
  std::unordered_map<unsigned, unsigned> rr_bitstream_sizes;
  rr_bitstream_sizes.reserve(rr_spec_map_.size());

  // Gather the RR's bitstream sizes from the first module of each RR.
  for (const auto rr : rr_spec_map_) {
    const auto rr_id = rr.first;

    // Assert that the current RR has at least one entry.
    if (rr.second.size() == 0) {
      throw std::runtime_error("Target RR has no modules to draw data from.");
    }

    // Get the RR's bitstream size from one of its modules.
    const auto bitstream_size = rr.second.begin()->second.bitstream_width;
    rr_bitstream_sizes[rr_id] = bitstream_size;
  }
  return rr_bitstream_sizes;
}

double application::getReconfigurableRegionSpeed(unsigned region_id, unsigned module_id) {
  const auto target_region = getTargetRrIt(region_id);

  // Assert that the target RR has at least one entry.
  if (target_region->second.size() == 0) {
    throw std::runtime_error("Target RR has no modules to draw data from.");
  }
  const auto rr_modules = target_region->second;
  const auto target_module = rr_modules.find(module_id);

  // Assert that the target module exists.
  if (target_module == rr_modules.end()) {
    throw std::runtime_error("Target RR module not found in map.");
  }
  return target_module->second.speed;
}

rrSpecMap_t& application::getReconfigurableRegions() {
  return rr_spec_map_;
}

rrSpecMap_t::iterator application::getTargetRrIt(unsigned rr_id) {
  const auto target_region = rr_spec_map_.find(rr_id);

  // Assert that the target RR exists.
  if (target_region == rr_spec_map_.end()) {
    throw std::runtime_error("Target RR not found in map.");
  }
  return target_region;
}

void printIndentation(unsigned indents) {
  for (unsigned i = 0; i < indents; i++) {
    std::cout << "\t";
  }
}

void application::printDetails(unsigned indents) {

  printIndentation(indents);
  std::cout << "There are " << static_modules_->size() << " static regions, operating at "
        << static_region_speed_ << " MHz\n";
  printIndentation(indents);

  std::cout << "with each having a resident module count of: { ";
  for (auto static_it = static_modules_->begin(); static_it != static_modules_->end();) {
    std::cout << static_it->second;

    if (++static_it != static_modules_->end())
      std::cout << ", ";
  }
  std::cout << " }\n\n";

  printIndentation(indents);
  std::cout << "There are " << rr_spec_map_.size() << " reconfigurable regions\n";
  printIndentation(indents);
  std::cout << "with each region having a bitstream size of: { ";

  for (auto it = rr_spec_map_.begin(); it != rr_spec_map_.end();) {
    const auto rr_bitstream_size = it->second.begin()->second.bitstream_width;
    std::cout << rr_bitstream_size << " kB";

    // Add a comma at the end of the line if not the last element.
    if (++it != rr_spec_map_.end()) {
      std::cout << ", ";
    }
  }
  std::cout << " }\n";

  for (const auto rr_entry : rr_spec_map_) {
    printIndentation(indents);

    std::cout << "RR" << rr_entry.first << "(f) = { ";
    const auto module_map = rr_entry.second;
    for (auto it = module_map.begin(); it != module_map.end();) {
      std::cout << it->second.speed << " MHz";

      // Add a comma at the end of the line if not the last element.
      if (++it != module_map.end()) {
        std::cout << ", ";
      }
    }

    const auto bitstream_size = rr_entry.second.begin()->second.bitstream_width;
    std::cout << " } of size " << bitstream_size << " kB\n";
  }
}
