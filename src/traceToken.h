#ifndef TRACE_TOKEN
#define TRACE_TOKEN

#include <string>
#include <iostream>
#include <sstream>

class traceToken;

std::ostream& operator<<(std::ostream& output, const traceToken* trace);
bool operator>(const traceToken &t1, const traceToken &t2);

class traceToken {

  bool addressing_static_region_;
  bool is_dependent_;

  unsigned region_id_;
  unsigned module_id_;
  unsigned request_time_;
  unsigned execution_time_;
  unsigned dependency_id_;

public:
  traceToken();
  traceToken(std::string trace_string);
  // ~traceToken() {};

  bool isForStaticRegion();
  bool isDependent();

  unsigned getRegionId();
  unsigned getModuleId();
  unsigned getRequestTime();
  unsigned getExectionTime();
  unsigned getDependencyId();

  void printTokenDetails();

  friend std::ostream& operator<<(std::ostream&, const traceToken*);
  friend bool operator>(const traceToken &t1, const traceToken &t2);
};

#endif
