#ifndef _READER_H_
#define _READER_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

class reader{

	public:
		reader(std::string input, std::vector<std::string>& accept);

		bool isValidFile();

		void parseEntries();

		void printEntries();

	private:

		std::string input_file_;

		char comment_ = '#';
		char long_comment_ = '/';

		std::vector<std::string> acceptable_args_;
		std::vector<std::string> file_body_;

		std::multimap<std::string, std::string> param_dictionary_;

};

#endif
