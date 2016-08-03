#ifndef _TRACE_TOKEN_
#define _TRACE_TOKEN_

#include "traceToken.h"


/* PUBLIC */

	traceToken::traceToken(std::string trace_string) {

		std::size_t next_comma = trace_string.find(",");
		std::string region_id = trace_string.substr(0, next_comma);

		if (region_id.at(0) == 's') {
			region_id = region_id.substr(1);
			addressing_static_region_ = true;

		} else
			addressing_static_region_ = false;

		region_id_ = std::stol(region_id);

		trace_string = trace_string.substr(next_comma + 1);
		next_comma = trace_string.find(",");

		module_id_ = std::stol( trace_string.substr(0, next_comma) );

		trace_string = trace_string.substr(next_comma + 1);
		next_comma = trace_string.find(",");

		request_time_ = std::stol( trace_string.substr(0, next_comma) );

		trace_string = trace_string.substr(next_comma + 1);
		next_comma = trace_string.find(",");

		execution_time_ = std::stol( trace_string.substr(0, next_comma) );

		trace_string = trace_string.substr(next_comma + 1);

		if (trace_string.length() != 0) {
			is_dependent_ = true;
			dependency_id_ = std::stol(trace_string);
		} else
			is_dependent_ = false;
	}

	void traceToken::printTokenDetails() {
		if (addressing_static_region_)
			std::cout << "s";

		std::cout << region_id_ << ", " << module_id_ << ", " << request_time_ << ", " << execution_time_;

		if (is_dependent_)
			std::cout << ", " << dependency_id_;

		std::cout << "\n";
	}

	std::ostream& operator<<(std::ostream& output, const traceToken* trace) {

		if (trace->addressing_static_region_)
			output << "s";

		output << trace->region_id_ << ", "
			   << trace->module_id_ << ", "
			   << trace->request_time_ << ", "
			   << trace->execution_time_;

		if (trace->is_dependent_)
			output << ", " << trace->dependency_id_;

	    return output;
	}

	// overload the greater than operator
	bool operator>(const traceToken &t1, const traceToken &t2) {
		return t1.request_time_ > t2.request_time_;
	}


#endif
