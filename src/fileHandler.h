#ifndef _FILE_HANDLER_H_
#define _FILE_HANDLER_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#define COMMENT_TAG '#'
#define LONG_COMMENT_TAG '/'


class fileHandler{

	public:
		// Constructors //
		fileHandler(){};

		fileHandler(std::string input, std::vector<std::string> accept); // desired paramters only

		fileHandler(std::string input, std::vector<std::string> accept, std::vector<std::string> regex); // desired paramters and data body regex


		// Enumeration
		enum lineType {UNKNOWN, WHITE, COMMENT, PARAMETER, DATA, INVALID};


		// Operations Functions //
		void parseContent(std::string fin);

		void parseEntries(); // search for accepted parameters and regex matches for data body

		void printEntries();

		void addToFile(std::string line); // todo: needs line validation

		std::pair<bool, unsigned long> findInFile(std::string line); // todo: not finished

		bool removeFromFile(std::string line); // todo: not finished


		// Accessor Methods //
		bool isFileValid();

		std::multimap<std::string, std::string> getParams();

		std::vector<std::string> getData();

		std::string input_file_;

	private:
		bool is_valid_ = true;

		std::vector<std::string> acceptable_params_;
		std::vector<std::string> regex_args_;

		std::vector< std::pair<lineType, std::string> > file_body_;

		std::multimap<std::string, std::string> param_dictionary_;

};

#endif
