#include "wallet.hpp"

/* PUBLIC */

wallet::wallet() {

  wallet_handler_ = fileHandler("wallet.wal", wallet_params_);

  if (wallet_handler_.isFileValid()) {
    auto wallet_contents = wallet_handler_.getParams();

    for (auto it = wallet_contents.begin(); it != wallet_contents.end(); ++it) {

      std::string next[] = { it->first, it->second };

      /*if (next[0].substr(0, 1) == "d") { // read in device file

        std::vector<std::string> dev_params {"family", "model", "slices"};
        fileHandler dev_reader = fileHandler(next[1], dev_params);

        device new_dev = device(dev_reader);
        devices_.push_back(new_dev);

      } else*/
      if (next[0].substr(0, 1) == "m") { // read in memory file

        auto new_mem = buildMemoryHierarchy(next[1]);
        memories_.push_back(new_mem);

      } else if (next[0].substr(0, 1) == "a") { // read in application file

        auto new_app = createApplication(next[1]);
        applications_.push_back(new_app);

      } else if (next[0].substr(0, 1) == "t") { // trace file list

        auto new_trace = createTraceFilePair(next[1]);
        trace_files_.push_back(new_trace);
      }
    }

  } else {
    std::cout << "\n\nERROR: BAD WALLET FILE!\n\n";
    throw;
  }
}


void wallet::printDetails() {

  /*std::cout << "The wallet contains " << devices_.size() << " devices, "
        << memories_.size() << " memories, and " << trace_files_.size() << " trace files.\n";

  std::cout << "\n\t== DEVICE LIST (with ID): == \n";
  for (int i = 0; i < devices_.size(); i++)
    std::cout << "\t\t" << i << ") '" << devices_[i].name_ << "' <" << devices_[i].file_ << ">\n";*/

  std::cout << "The wallet contains " << memories_.size() << " memory configurations, "
        << applications_.size() << " applications, and " << trace_files_.size() << " trace files.\n";

  std::cout << "\n\n\t== MEMORY LIST (with ID): ==\n";
  for (auto i = 0; i < memories_.size(); i++) {

    std::cout << "\t\t" << i << ") '" << memories_[i].name_ << "' <" << memories_[i].file_ << ">\n";

    storageUnit* ptr = &(memories_[i]);
    while (ptr) {
      ptr->printDetails();
      ptr = ptr->getChild();
    }

    std::cout << "\n";
  }

  std::cout << "\n\t== APPLICATION LIST (with ID): ==\n";
  for (auto i = 0; i < applications_.size(); i++) {
    std::cout << "\t\t" << i << ") '" << applications_[i]->name_ << "' <" << applications_[i]->file_ << ">\n";
    applications_[i]->printDetails(3);
  }


  std::cout << "\n\n\t== TRACE FILE LIST (with ID): ==\n";
  for (auto i = 0; i < trace_files_.size(); i++) {
    std::cout << "\t\t" << i << ") '" << trace_files_[i]->second << "' <" << trace_files_[i]->first << ">\n";
  }

  std::cout << std::endl;
}

void wallet::addMemory(std::string file_name) {
  wallet_handler_.addToFile("m: " + file_name);
}

bool wallet::removeMemory(std::string file_name) {
  return wallet_handler_.removeFromFile("m: " + file_name);
}

void wallet::addApplication(std::string file_name) {
  wallet_handler_.addToFile("a: " + file_name);
}

bool wallet::removeApplication(std::string file_name) {
  return wallet_handler_.removeFromFile("a: " + file_name);
}

void wallet::addTraceFile(std::string file_name) {
  wallet_handler_.addToFile("t: " + file_name);
}

bool wallet::removeTraceFile(std::string file_name) {
  return wallet_handler_.removeFromFile("t: " + file_name);
}

std::vector<std::string> wallet::getMemoryParamRules() {
  return memory_params_;
}

std::vector<std::string> wallet::getMemoryRegexRules() {
  return memory_regex_;
}

std::vector<std::string> wallet::getApplicationParamRules() {
  return application_params_;
}

std::vector<std::string> wallet::getApplicationRegexRules() {
  return application_regex_;
}

std::vector<std::string> wallet::getTraceParamRules() {
  return trace_params_;
}

std::vector<std::string> wallet::getTraceRegexRules() {
  return trace_regex_;
}


reconfigurableRegions wallet::getMemory(unsigned memory_id) {
//std::string wallet::getMemory(unsigned memory_id) {
  return memories_[memory_id];
}

application* wallet::getApplication(unsigned application_id) {
  return applications_[application_id];
}

std::vector<std::string> wallet::getTraceFile(unsigned trace_id) {
  auto trace_reader = fileHandler(trace_files_[trace_id]->first, trace_params_, trace_regex_);

  return trace_reader.getData();
}


/* PRIVATE */

reconfigurableRegions wallet::buildMemoryHierarchy(std::string memory_file) {

  auto memory_reader = fileHandler(memory_file, memory_params_, memory_regex_);

  if (!memory_reader.isFileValid()) {
    std::cout << "\n\nMEMORY FILE IS CORRUPT!!!\n\n";
    throw;
  }

  // start by retrieving the information needed to form the main menu and pr regions
  auto memory_contents = memory_reader.getParams();
  std::string mem_name, main_mem_name, main_mem_size, main_mem_read, main_mem_search; // prr_count

  for (auto it = memory_contents.begin(); it != memory_contents.end(); ++it) {

    std::string next[] = { it->first, it->second };

    if (next[0] == "name") { // read in memory configuration's "name"
      mem_name = next[1];

    } else if (next[0] == "main name") { // read in main memory name
      main_mem_name = next[1];

    } else if (next[0] == "main size") { // read in main memory size
      main_mem_size = next[1];

    } else if (next[0] == "main read latency") { // read in main memory read latency (in CCs)
      main_mem_read = next[1];

    } else if (next[0] == "main search latency") { // read in main memory search latency (in CCs)
      main_mem_search = next[1];
    }

    //else if (next[0] == "rcr count") // read in amount of reconfigurable regions
    //  prr_count = next[1];
  }

  // initialize the highest memory level, the pr regions
  reconfigurableRegions prr = reconfigurableRegions();
  prr.name_ = mem_name;
  prr.file_ = memory_file;

  // initialize the lowest level, the main memory
  memoryLevel* main_memory = new memoryLevel(
    std::stol(main_mem_size),
    NULL,
    (unsigned short) std::stol(main_mem_read),
    (unsigned short) std::stol(main_mem_search)
  );
  main_memory->name_ = main_mem_name;

  // initialize main memory's modules
  //for (unsigned i = 0; i < std::stol(main_mem_size); i++)
  //  main_memory->insertModule( new module(i, 777) );
      // TODO: bitstream width is arbitrary since it is not yet considered in Drachma

  // create interstitial levels (up to L9) of cache based on data parameters
  auto level_params = memory_reader.getData();
  std::string level_name[10];
  int level_size[10], level_read[10], level_search[10], highest_level = 0;

  bool lut_indexed[10];
  for (auto i = 0; i < 10; i++) {
    lut_indexed[i] = false;
  }

  for (auto i = 0; i < level_params.size(); i++) {

    auto which_level = atoi(level_params[i].substr(1, 1).c_str()); // grab level number
    auto param = level_params[i].substr(3, level_params[i].find(":") - 3); // grab rest of param
    auto arg = level_params[i].substr(level_params[i].find(":") + 2); // grab param value

    if (highest_level < which_level) { // keep track of the highest level declared
      highest_level = which_level;
    }

    if (param == "name") { // read in level's name
      level_name[which_level] = arg + " [L" + std::to_string(which_level) + "]";

    } else if (param == "size") { // read in level's size
      level_size[which_level] = std::stol(arg);

    } else if (param == "read latency") { // read in level's read latency (in CCs)
      level_read[which_level] = std::stol(arg);

    } else if (param == "search latency") // read in level's search latency (in CCs)
      level_search[which_level] = std::stol(arg);

    else if (param == "has LUT") {
      std::cout << ">>> L" << which_level << " has LUT <<<\n\n";
      lut_indexed[which_level] = true;
    }

    // TODO: perform check for incomplete cache level parameters
  }

  // create a hierarchy-building pointer and aim it at the highest level (the pr regions)
  storageUnit* parent = &prr;

  // if cache levels have been declared, link them all together starting from the lowest level
  if (highest_level > 0) {

    for (auto i = 1; i <= highest_level; i++) {

      // TODO: make it so the memory files are used to list the algorithm for each cache level

      // use only random replacement
      //replAlg* default_alg = new randomAlg("Random Replacement", level_size[i]);
      // use only fifo replacement
      auto default_alg = new fifoAlg("FIFO Replacement", level_size[i]);

      auto child = new memoryLevel(
        level_size[i],
        default_alg,
        level_read[i],
        level_search[i],
        lut_indexed[i]
      );

      child->name_ = level_name[i];

      parent->setChild(child);
      parent = child;
    }
  }

  // lastly link the main memory to the bottom of the hierarchy
  parent->setChild(main_memory);

  // return hierarchy's top level
  return prr;
}

application* wallet::createApplication(std::string application_file) {

  auto application_reader = fileHandler(application_file, application_params_, application_regex_);

  if (!application_reader.isFileValid()) {
    std::cout << "\n\nAPPLICATION FILE IS CORRUPT!!!\n\n";
    throw;
  }

  // start by retrieving the basic application parameters
  auto statically_defined_params = application_reader.getParams();

  // get the application's name
  auto application_name = statically_defined_params.find("name")->second;

  // get icap speed and bus width, then instantiate the icap
  auto icap_speed = statically_defined_params.find("icap speed")->second;
  auto icap_width = statically_defined_params.find("icap width")->second;
  auto application_icap = new icap(std::stod(icap_speed), std::stol(icap_width));

  // get prc speed, then instantiate the prc
  auto prc_speed = statically_defined_params.find("prc speed")->second;
  auto application_prc = new prc(std::stod(prc_speed));

  // get static region speed, if a static region has been defined
  std::string static_region_speed = "";
  if (statically_defined_params.find("static region speed") != statically_defined_params.end()) {
    static_region_speed = statically_defined_params.find("static region speed")->second;
  }

  // speed of fastest module dictates the speed of the simulator. start search with static region.
  auto fastest_module_speed = std::stod(static_region_speed);

  auto static_modules = new std::multimap<unsigned, unsigned>();
  auto reconfigurable_modules = new std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>();

  // start parsing the arbitrary amounts of static and reconfigurable regions
  auto dynamically_declared_params = application_reader.getData();
  for (auto i = 0; i < dynamically_declared_params.size(); i++) {
    //std::cout << dynamically_declared_params[i] << "\n";

    // get the parameter and the corresponding argument
    auto param = dynamically_declared_params[i].substr(0, dynamically_declared_params[i].find(":"));
    auto arg = dynamically_declared_params[i].substr(dynamically_declared_params[i].find(":") + 2);

    //std::cout << "> " << param << ", " << arg << "\n\n";

    // handle static region parameters
    if (param.substr(0, 2) == "sr") {
      unsigned region_id_end_index = param.find(" ");
      auto region_id = param.substr(2, region_id_end_index - 2);

      static_modules->insert(std::pair<unsigned, unsigned>(std::stol(region_id), std::stol(arg)));
    }

    // handle reconfigurable region parameters
    if (param.substr(0, 2) == "rr") {
      unsigned region_id_end_index = param.find(" ");
      auto region_id = std::stol(param.substr(2, region_id_end_index - 2));

      // get reconfigurable region bitstream size
      if (param.substr(region_id_end_index + 1, 3) == "bit") {
        std::pair<unsigned, std::vector<double>> new_region_params;
        new_region_params.first = std::stol(arg);

        auto new_region
          = std::pair<unsigned, std::pair<unsigned, std::vector<double>>>(region_id, new_region_params);
        reconfigurable_modules->insert(new_region);

      // get module speed
      } else {
        auto region_it = reconfigurable_modules->find(region_id);

        unsigned module_id_start_index = param.find("module") + 7;
        unsigned module_id_end_index = param.find(" ", module_id_start_index);
        auto module_id = param.substr(module_id_start_index, module_id_end_index - module_id_start_index);

        //(region_it->second)[std::stol(module_id)] = std::stod(arg);
        region_it->second.second.emplace(region_it->second.second.begin() + std::stol(module_id), std::stod(arg));

        // continue search for the fastest module.
        if (std::stod(arg) > fastest_module_speed) {
          fastest_module_speed = std::stod(arg);
        }
      }
    }

  }


  ////////// STARTING OF PRINT CODE

  /*std::multimap<unsigned, unsigned>::iterator static_it;
  std::cout << "There are " << static_modules.size() << " static regions, all operating at a speed of "
        << static_region_speed << " MHz\n";
  for (static_it = static_modules.begin(); static_it != static_modules.end(); static_it++)
    std::cout << "> Static Region " << static_it->first << " has " << static_it->second << " modules.\n";

  std::multimap<unsigned, std::pair<unsigned, std::vector<double>>>::iterator reconfig_it;
  for (reconfig_it = reconfigurable_modules.begin(); reconfig_it != reconfigurable_modules.end(); reconfig_it++) {
    std::cout << "\nReconfigurable Region " << reconfig_it->first << " has a bitstream size of "
          << reconfig_it->second.first << " kB.\n";

    for (unsigned it = 0; it < reconfig_it->second.second.size(); it++) {
      std::cout << "> Reconfigurable Module " << reconfig_it->first << "." << it
            << " operates at a speed of " << reconfig_it->second.second[it] << " MHz.\n";
    }
  }*/
  ///////// END OF PRINT CODE

  auto new_app = new application(
    application_icap,
    application_prc,
    std::stod(static_region_speed),
    static_modules,
    reconfigurable_modules,
    fastest_module_speed
  );

  new_app->name_ = application_name;
  new_app->file_ = application_file;
  //new_app->hello();

  return new_app;
}

std::pair<std::string, std::string>* wallet::createTraceFilePair(std::string trace_file) {

  auto trace_reader = fileHandler(trace_file, trace_params_, trace_regex_);

  if (!trace_reader.isFileValid()) {
    std::cout << "\n\nTRACE FILE IS CORRUPT!!!\n\n";
    throw;
  }

  auto trace_name = trace_reader.getParams().find("name")->second;
  return new std::pair<std::string, std::string>(trace_file, trace_name);
}
