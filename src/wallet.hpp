#ifndef NALLJ_DRACHMA_WALLET
#define NALLJ_DRACHMA_WALLET

#include <iostream>
#include <map> // multimap
#include <memory> // make_shared, shared_ptr
#include <string> // getline, string, to_string
#include <sstream> // stringstream
#include <stdexcept> // runtime_error
#include <unordered_map> // unordered_map
#include <utility> // move, pair
#include <vector>

#include <boost/algorithm/string/trim.hpp>
#include <cppJsonGraph/deserializer.hpp>
#include <cppJsonGraph/graph.hpp>

#include <yaml-cpp/yaml.h>

#include "algs/cache/fifoAlg.hpp" // fifo replacement
//#include "algs/cache/randomAlg.hpp" // random replacement
#include "components/icap.hpp"
#include "components/prc.hpp"
#include "device.hpp"
#include "fileHandler.hpp"
#include "specs/application.hpp"
#include "specs/application-yaml.hpp"
#include "storage/storageUnit.hpp"
#include "storage/memoryLevel.hpp"
#include "storage/reconfigurableRegions.hpp"
#include "types.hpp"
#include "userError.hpp"

using graph_ptr_t = std::shared_ptr<nallj::graph>;

// template<>
// struct YAML::convert<application>;

// wallet acts as a library for drachma
class wallet {

  // file format rules
  std::vector<std::string> wallet_params_ { "name", "m", "a", "t" };
  std::vector<std::string> memory_params_ {
    "name",
    "main name",
    "main size",
    "main read latency",
    "main search latency"
  };
  std::vector<std::string> memory_regex_ {
    "[l][1-9] \\bname\\b[:] [\\w ]+",
    "[l][1-9] \\bsize\\b[:] [\\w ]+",
    "[l][1-9] \\b(read|search)\\b \\blatency\\b[:] [\\w ]+",
    "[l][1-9] has LUT"
  };

  std::vector<std::string> application_params_ {
    "name",
    "icap width",
    "icap speed",
    "prc speed",
    "rr selection policy",
    "task scheduling",
    "static region speed"
  };
  std::vector<std::string> application_regex_ {
    "\\bsr\\d+ \\bmodule count\\b[:] \\d+",
    "\\brr\\d+\\b \\bbitstream size\\b[:] \\d+",
    "\\brr\\d+\\b \\bmodule \\d+ speed\\b[:] \\d+[.]\\d{1,2}",
    "\\brr\\d+\\b \\bmodule \\d+ task types\\b[:] [^,\\n]+(, *[^,\\n ]+)*"
  };

  std::vector<std::string> trace_params_ { "name" };
  std::vector<std::string> trace_regex_ {
    //"s?(\\d+)(,\\s*\\d+) {3},",
    //"s?(\\d+)(,\\s*\\d+) {4}"
    "s?(\\d+)(,\\d+) {3},(\\d+)?"
  };

  std::vector<reconfigurableRegions> memories_;
  std::vector<std::shared_ptr<application>> applications_;
  std::vector<std::pair<std::string, graph_ptr_t>> task_graphs_files_;
  fileHandler wallet_file_handler_;

  void assertEntityFileExists(const std::string& filename, const std::string& entity_name);
  reconfigurableRegions buildMemoryHierarchy(const std::string& memory_file);
  std::shared_ptr<application> createApplication(const std::string& application_file);
  graph_ptr_t createTaskGraph(const std::string& tg_file);

public:
  std::string name_;

  wallet();

  void addApplication(const std::string& filename);
  void addMemory(const std::string& filename);
  void addTraceFile(const std::string& filename);
  std::shared_ptr<application> getApplication(unsigned application_id);
  std::vector<std::string> getApplicationParamRules();
  std::vector<std::string> getApplicationRegexRules();
  reconfigurableRegions getMemory(unsigned memory_id);
  std::vector<std::string> getMemoryParamRules();
  std::vector<std::string> getMemoryRegexRules();
  graph_ptr_t getTaskGraph(unsigned task_graph_id);
  std::vector<std::string> getTraceParamRules();
  std::vector<std::string> getTraceRegexRules();
  void printDetails();
  bool removeApplication(const std::string& filename);
  bool removeMemory(const std::string& filename);
  bool removeTaskGraphFile(const std::string& filename);
};

#endif
