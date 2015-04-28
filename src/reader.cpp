#ifndef _READER_
#define _READER_

#include "reader.h"

reader::reader(std::string input, std::vector<std::string> accept) :
	input_file_(input), acceptable_params_(accept){

	file_body_ = parseContent(input);

	parseEntries();

	if(is_valid_) printEntries();
	else{
		std::cout << "\nYOU HAVE ANGERED THE PARSING GOD!!!\n";
		printEntries();
	}
}

reader::reader(std::string input, std::vector<std::string> accept, std::vector<std::string> regex) :
		input_file_(input), acceptable_params_(accept), regex_args_(regex){

	file_body_ = parseContent(input);

	parseEntries();

	if(is_valid_) printEntries();
	else{
		std::cout << "\nYOU HAVE ANGERED THE PARSING GOD!!!\n";
		printEntries();
	}
}

std::vector<std::string> reader::parseContent(std::string fin){

	std::vector<std::string> body;
	std::ifstream in_file(fin.c_str());
	std::string line;
	bool comment_ignore = false;

	while(std::getline(in_file, line)){

		// start of comment block
		if(line.length() > 1 && line.at(0) == LONG_COMMENT && line.at(1) == COMMENT){

			comment_ignore = true;
			continue;
		}

		// parsing not inhibited by comment block
		if(line.length() != 0 && !comment_ignore){

			// single line comment
			if(line.length() > 0 && line.at(0) == COMMENT)
				continue;

			// push non-disqualified line onto file body holder
			body.push_back(line);
		}

		// end a comment block
		if(comment_ignore && line.length() > 1 && line.at(0) == COMMENT && line.at(1) == LONG_COMMENT)
			comment_ignore = false;
	}
	in_file.close();

	return body;
}

bool reader::isValidFile(){ return is_valid_; }

void reader::parseEntries(){

	for(int i = 0; i < file_body_.size(); i++){

		std::size_t pos = file_body_[i].find(':');

		if(pos != std::string::npos){ // presence of colon implies (but doesn't guarantee) an acceptable parameter

			std::string test_param = file_body_[i].substr(0, pos);
			std::string test_arg = file_body_[i].substr(pos + 1);

			boost::trim(test_arg);

			if(test_arg.length() == 0){ // check for parameter with no argument

				std::cout << "ERROR: This is not a valid file. <Parameter with No Argument>" << std::endl << std::endl;
				is_valid_ = false;
				break; // file invalid, stop parsing
			}

			// check if acceptable parameter is present
			if(std::find(acceptable_params_.begin(), acceptable_params_.end(), test_param) != acceptable_params_.end()
				&& test_arg.length() != 0){

				// multimap parameter and value pairs - like a dictionary
				std::pair<std::string, std::string> entry(test_param, test_arg);
				param_dictionary_.insert(entry);

				continue; // parameter accepted, next input
			}

		}



		bool pass_regex = false;
		for(int j = 0; j < regex_args_.size(); j++){

			boost::smatch match;

			if( boost::regex_match(file_body_[i], match, boost::regex{regex_args_[j]}) ){

				data_body_.push_back(file_body_[i]); // insert into data vector

				pass_regex = true;
				break; // regex match found, stop regex parsing
			}
		}

		if(pass_regex) continue; // data accepted, next input


		// getting this far means the line contains neither parameter nor data
		std::cout << "ERROR: This is not a valid file. <Line#" << i << " is Neither Parameter nor Data>" << std::endl << std::endl;
		is_valid_ = false;
		break;

	} // /file body search

}

void reader::printEntries(){

	std::cout << "> PARAMETER MULTIMAP <\n";
	for(std::multimap<std::string,std::string>::iterator it = param_dictionary_.begin(); it != param_dictionary_.end(); ++it)
		std::cout << "MAP: " << it->first << "  \t=>\t" << it->second << std::endl;

	std::cout << "\n> DATA VECTOR <" << std::endl;
	for(int i = 0; i < data_body_.size(); i++)
			 std::cout << "DATA: " << data_body_[i] << std::endl;

}

#endif
