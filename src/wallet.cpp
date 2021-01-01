#include "wallet.hpp"

wallet::wallet() {
  wallet_file_handler_ = fileHandler("wallet.wal", wallet_params_);

  if (wallet_file_handler_.isFileValid()) {
    const auto wallet_contents = wallet_file_handler_.getParams();

    for (const auto& file_data : wallet_contents) {

      const auto file_type = file_data.first[0];
      const auto filename = file_data.second;

      switch (file_type) {
        // Read in the memory file.
        case 'm': {
          assertEntityFileExists(filename, "Memory");

          const auto new_mem = buildMemoryHierarchy(filename);
          memories_.push_back(new_mem);
          break;
        }
        // Read in the application file.
        case 'a': {
          assertEntityFileExists(filename, "Application");

          const auto new_app = createApplication(filename);
          applications_.push_back(new_app);
          break;
        }
        // Read in task graph file.
        case 't': {
          assertEntityFileExists(filename, "Task graph");

          auto new_tg = createTaskGraph(filename);
          task_graphs_files_.push_back({ filename, std::move(new_tg) });
          break;
        }
      }
    }

  } else {
    throw userError("Wallet file provided has invalid format.");
  }
}

/* Helper Functions */

long getModuleIdFromAppParamLine(const std::string& param) {
  const auto start = param.find("module ") + 7;
  const auto end = param.find(" ", start);
  const auto module_id = param.substr(start, end - start);
  return std::stol(module_id);;
}

std::string getParamFromMap(const std::multimap<std::string, std::string> map, const char* key) {
  const auto find_it = map.find(key);
  if (find_it == map.end()) {
    throw std::runtime_error("Expected value not found in map.");
  }
  return find_it->second;
}

long getRrIdFromAppParamLine(const std::string& param) {
  const auto end = param.find(" ");
  const auto rr_id = param.substr(2, end - 2);
  return std::stol(rr_id);
}

long getSrIdFromAppParamLine(const std::string& param) {
  const auto end = param.find(" ");
  const auto sr_id = param.substr(2, end - 2);
  return std::stol(sr_id);
}

void pushCommaDelimitedElements(std::vector<std::string>& list, const std::string str) {
  auto ss = std::stringstream(str);
  while (ss.good()) {
    std::string element;
    std::getline(ss, element, ',');

    boost::trim(element);
    list.push_back(element);
  }
}

bool stringContains(const std::string& s, const std::string& substr) {
  return s.find(substr) != std::string::npos;
}

/* Methods */

void wallet::addApplication(const std::string& filename) {
  wallet_file_handler_.addToFile("a: " + filename);
}

void wallet::addMemory(const std::string& filename) {
  wallet_file_handler_.addToFile("m: " + filename);
}

void wallet::addTraceFile(const std::string& filename) {
  wallet_file_handler_.addToFile("t: " + filename);
}

void wallet::assertEntityFileExists(const std::string& filename, const std::string& entity_name) {
  auto file_exists = access(filename.c_str(), F_OK) != -1;
  if (!file_exists) {
    const auto msg = entity_name + " file referenced in wallet (" + filename + ") does not exist.";
    throw userError(msg);
  }
}

reconfigurableRegions wallet::buildMemoryHierarchy(const std::string& memory_file) {

  auto memory_reader = fileHandler(memory_file, memory_params_, memory_regex_);

  if (!memory_reader.isFileValid()) {
    throw userError("Memory file is corrupt.");
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

std::shared_ptr<application> wallet::createApplication(const std::string& application_file) {

  const auto& app = YAML::LoadFile(application_file).as<application>();
  const auto app_ptr = std::make_shared<application>(app);

  app_ptr->file = application_file;
  return app_ptr;
}

graph_ptr_t wallet::createTaskGraph(const std::string& tg_file) {
  nallj::deserializer d;
  auto input_stream = std::ifstream(tg_file);
  auto graph = d.getGraph(input_stream);
  return std::make_shared<nallj::graph>(graph);
}

std::shared_ptr<application> wallet::getApplication(unsigned application_id) {
  if (applications_.size() - 1 < application_id) {
    throw userError("Too large of an application ID was specified.");
  }
  return applications_[application_id];
}

std::vector<std::string> wallet::getApplicationParamRules() {
  return application_params_;
}

std::vector<std::string> wallet::getApplicationRegexRules() {
  return application_regex_;
}

reconfigurableRegions wallet::getMemory(unsigned memory_id) {
  if (memories_.size() - 1 < memory_id) {
    throw userError("Too large of a memory ID was specified.");
  }
  return memories_[memory_id];
}

std::vector<std::string> wallet::getMemoryParamRules() {
  return memory_params_;
}

std::vector<std::string> wallet::getMemoryRegexRules() {
  return memory_regex_;
}

graph_ptr_t wallet::getTaskGraph(unsigned task_graph_id) {
  if (task_graphs_files_.size() - 1 < task_graph_id) {
    throw userError("Too large of a task graph ID was specified.");
  }
  return task_graphs_files_[task_graph_id].second;
}

std::vector<std::string> wallet::getTraceParamRules() {
  return trace_params_;
}

std::vector<std::string> wallet::getTraceRegexRules() {
  return trace_regex_;
}

void wallet::printDetails() {
  std::cout << "The wallet contains " << memories_.size() << " memory configurations, "
    << applications_.size() << " applications, and " << task_graphs_files_.size()
    << " task graph files.\n";

  std::cout << "\n\t== MEMORY LIST (with ID): ==\n";
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
    std::cout << "\t\t" << i << ") '" << applications_[i]->name << "' <" << applications_[i]->file << ">\n";
    applications_[i]->printDetails(3);
  }

  std::cout << "\n\t== TASK GRAPH FILE LIST (with ID): ==\n";
  for (auto i = 0; i < task_graphs_files_.size(); i++) {
    const auto& tg_file = task_graphs_files_[i];
    const auto& filename = tg_file.first;
    const auto& task_graph = tg_file.second.get();

    std::cout << "\t\t" << i << ")";
    if (task_graph->getIdIsSet()) {
      std::cout << " " << task_graph->getId();
    }
    std::cout << " <" << filename << ">\n";
  }

  std::cout << std::endl;
}

bool wallet::removeApplication(const std::string& filename) {
  return wallet_file_handler_.removeFromFile("a: " + filename);
}

bool wallet::removeMemory(const std::string& filename) {
  return wallet_file_handler_.removeFromFile("m: " + filename);
}

bool wallet::removeTaskGraphFile(const std::string& filename) {
  return wallet_file_handler_.removeFromFile("t: " + filename);
}
