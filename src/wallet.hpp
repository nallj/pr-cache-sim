#ifndef WALLET_H
#define WALLET_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <map>

#include "fileHandler.hpp"
#include "device.hpp"
#include "application.hpp"

#include "storage/storageUnit.hpp"
#include "storage/memoryLevel.hpp"
#include "storage/reconfigurableRegions.hpp"

//#include "algs/randomAlg.hpp" // random replacement
#include "algs/fifoAlg.hpp" // fifo replacement

#include "components/icap.hpp"
#include "components/prc.hpp"


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
    "static region speed"
  };
  std::vector<std::string> application_regex_ {
    "\\bsr\\d+ \\bmodule count\\b[:] \\d+",
    "\\brr\\d+\\b \\bbitstream size\\b[:] \\d+",
    "\\brr\\d+\\b \\bmodule \\d+ speed\\b[:] \\d+[.]\\d{1,2}"
  };

  std::vector<std::string> trace_params_ { "name" };
  std::vector<std::string> trace_regex_ {
    //"s?(\\d+)(,\\s*\\d+) {3},",
    //"s?(\\d+)(,\\s*\\d+) {4}"
    "s?(\\d+)(,\\d+) {3},(\\d+)?"
  };

  fileHandler wallet_handler_;
  reconfigurableRegions buildMemoryHierarchy(std::string memory_file);
  application* createApplication(std::string application_file);
  std::pair<std::string, std::string>* createTraceFilePair(std::string trace_file);
  //std::vector<device> devices_;
  std::vector<reconfigurableRegions> memories_;
  std::vector<application*> applications_;
  std::vector<std::pair<std::string, std::string>*>trace_files_;

public:
  std::string name_;

  wallet();

  void printDetails();
  //void addDevice(std::string file_name);
  //bool removeDevice(std::string file_name);
  void addMemory(std::string file_name);
  bool removeMemory(std::string file_name);
  void addApplication(std::string file_name);
  bool removeApplication(std::string file_name);
  void addTraceFile(std::string file_name);
  bool removeTraceFile(std::string file_name);

  std::vector<std::string> getMemoryParamRules();
  std::vector<std::string> getMemoryRegexRules();
  std::vector<std::string> getApplicationParamRules();
  std::vector<std::string> getApplicationRegexRules();
  std::vector<std::string> getTraceParamRules();
  std::vector<std::string> getTraceRegexRules();

  //device getDevice(unsigned device_id);
  reconfigurableRegions getMemory(unsigned memory_id);
  application* getApplication(unsigned application_id);
  std::vector<std::string> getTraceFile(unsigned trace_id);
};

#endif
