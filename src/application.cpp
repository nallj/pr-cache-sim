#include "application.hpp"

application::application() { }
application::application(
  icap* application_icap,
  prc* application_prc,
  double static_region_speed,
  std::multimap<unsigned, unsigned>* static_modules,
  std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>* reconfigurable_modules,
  double fastest_module_speed
) :
  application_icap_(application_icap), application_prc_(application_prc),
  static_region_speed_(static_region_speed), static_modules_(static_modules),
  reconfigurable_modules_(reconfigurable_modules),
  simulator_speed_(fastest_module_speed) { }
application::~application() {
  delete application_icap_;
  delete application_prc_;
  delete static_modules_;
  delete reconfigurable_modules_;
}


void application::printApplicationDetails() {
  std::cout << "\n\nI don't know what I got yet, man.\n\n";
}

icap* application::getIcap() {
  return application_icap_;
}
prc* application::getPrc() {
  return application_prc_;
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
std::multimap<unsigned, unsigned>* application::getStaticModules() {
  return static_modules_;
}

unsigned application::getReconfigurableRegionCount() {
  return reconfigurable_modules_->size();
}

unsigned application::getReconfigurableRegionBitstreamSize(unsigned region_id) {
  std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>::iterator target_region
    = reconfigurable_modules_->find(region_id);

  // return the region bitstream width if found.  if not, return -1 as an error code.
  if (target_region != reconfigurable_modules_->end())
    return target_region->second.first;
  else
    return -1;
}

std::vector<unsigned> application::getReconfigurableRegionBitstreamSizes() {
  std::vector<unsigned> bitstream_sizes;

  for (unsigned i = 0; i < reconfigurable_modules_->size(); i++)
    bitstream_sizes.push_back(reconfigurable_modules_->find(i)->second.first);

  return bitstream_sizes;
}

double application::getReconfigurableRegionSpeed(unsigned region_id, unsigned module_id) {
  std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>::iterator target_region
    = reconfigurable_modules_->find(region_id);

  // target the region if found.  if not, return -1 as an error code.
  if (target_region != reconfigurable_modules_->end())
    return target_region->second.second[module_id];

  else
    return -1.0;
}

std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>* application::getReconfigurableModules() {
  return reconfigurable_modules_;
}

void application::printDetails(unsigned indents) {

  for (unsigned i = 0; i < indents; i++)
    std::cout << "\t";

  std::multimap<unsigned, unsigned>::iterator static_it;
  std::cout << "There are " << static_modules_->size() << " static regions, operating at "
        << static_region_speed_ << " MHz\n";


  for (unsigned i = 0; i <= indents; i++)
    std::cout << "\t";

  std::cout << "with each having a resident module count of: { ";
  for (static_it = static_modules_->begin(); static_it != static_modules_->end();) {
    std::cout << static_it->second;

    if (++static_it != static_modules_->end())
      std::cout << ", ";
  }
  std::cout << " }\n\n";


  for (unsigned i = 0; i < indents; i++)
    std::cout << "\t";
  std::cout << "There are " << reconfigurable_modules_->size() << " reconfigurable regions\n";
  for (unsigned i = 0; i <= indents; i++)
          std::cout << "\t";
  std::cout << "with each region having a bitstream size of: { ";

  std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>::iterator reconfig_it;
  for (reconfig_it = reconfigurable_modules_->begin(); reconfig_it != reconfigurable_modules_->end();) {
    std::cout << reconfig_it->second.first << " kB";

    if (++reconfig_it != reconfigurable_modules_->end())
      std::cout << ", ";
  }
  std::cout << " }\n";

  for (reconfig_it = reconfigurable_modules_->begin(); reconfig_it != reconfigurable_modules_->end(); reconfig_it++) {
    for (unsigned i = 0; i <= indents; i++)
      std::cout << "\t";

    std::cout << "RR" << reconfig_it->first << "(f) = { ";
    for (unsigned it = 0; it < reconfig_it->second.second.size();) {
      std::cout << reconfig_it->second.second[it] << " MHz";

      if (++it != reconfig_it->second.second.size())
        std::cout << ", ";
    }

    std::cout << " }\n";
  }
}
