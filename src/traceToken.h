#ifndef _TRACE_TOKEN_H_
#define _TRACE_TOKEN_H_

#include <string>
#include <iostream>
#include <sstream>

class traceToken;
std::ostream& operator<<(std::ostream& output, const traceToken* trace);
bool operator>(const traceToken &t1, const traceToken &t2);


class traceToken {

	public:
		traceToken() {};
		traceToken(std::string trace_string);
		~traceToken() {};

		bool isForStaticRegion() { return addressing_static_region_; }
		bool isDependent() { return is_dependent_; }

		unsigned getRegionId() { return region_id_; }
		unsigned getModuleId() { return module_id_; }
		unsigned getRequestTime() { return request_time_; }
		unsigned getExectionTime() { return execution_time_; }
		unsigned getDependencyId() { return dependency_id_; }

		void printTokenDetails();

		friend std::ostream& operator<<(std::ostream&, const traceToken*);
		friend bool operator>(const traceToken &t1, const traceToken &t2);

	private:
		bool addressing_static_region_;
		bool is_dependent_;

		unsigned region_id_;
		unsigned module_id_;
		unsigned request_time_;
		unsigned execution_time_;
		unsigned dependency_id_;

};

#endif
