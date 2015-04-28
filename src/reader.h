#ifndef _READER_H_
#define _READER_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#define COMMENT '#'
#define LONG_COMMENT '/'


class reader{

	public:
		reader(std::string input, std::vector<std::string> accept); // desired paramters only

		reader(std::string input, std::vector<std::string> accept, std::vector<std::string> regex); // desired paramters and data body regex


		std::vector<std::string> parseContent(std::string fin); //, std::vector<std::string> data_criteria);

		void parseEntries(); // search for accepted parameters and regex matches for data body

		void printEntries();


		bool isFileValid();

		std::multimap<std::string, std::string> getParams();

		std::vector<std::string> getData();

	private:

		std::string input_file_;

		bool is_valid_ = true;

		std::vector<std::string> acceptable_params_;
		std::vector<std::string> regex_args_;

		std::vector<std::string> file_body_;
		std::vector<std::string> data_body_;

		std::multimap<std::string, std::string> param_dictionary_;

};

#endif
