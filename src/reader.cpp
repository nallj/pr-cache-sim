#ifndef _READER_
#define _READER_

#include "reader.h"


reader::reader(std::string input, std::vector<std::string>& accept) :
	input_file_(input), acceptable_args_(accept){

	std::ifstream in_file(input.c_str());
	std::string line;
	bool comment_ignore = false;

	while(std::getline(in_file, line)){

		// start of comment block
		if(line.length() > 1 && line.at(0) == long_comment_ && line.at(1) == comment_){
			comment_ignore = true;
			continue;
		}

		// parsing not inhibited by comment block
		if(line.length() != 0 && !comment_ignore){

			// single line comment
			if(line.length() > 0 && line.at(0) == comment_)
				continue;

			// push non-disqualified line onto file body holder
			file_body_.push_back(line);
		}

		// end a comment block
		if(comment_ignore && line.length() > 1 && line.at(0) == comment_ && line.at(1) == long_comment_)
			comment_ignore = false;
	}

}

bool reader::isValidFile(){
	bool valid = true;

	for(int i = 0; i < file_body_.size(); i++){

		int pos;
		for(int j = 0; j < acceptable_args_.size(); j++){

			pos = file_body_[i].find(acceptable_args_[j] + ": ");

			// parameter name found at beginning
			if(pos == 0)
				break;

			// if no match is found return false
			if(j == acceptable_args_.size() - 1){
				valid = false;
				break;
			}
		}

		if(!valid)
			break;
	}

	return valid;
}

void reader::parseEntries(){

	std::size_t pos;

	for(int i = 0; i < file_body_.size(); i++){

		pos = file_body_[i].find(':');

		if (pos == std::string::npos)
			param_dictionary_.clear();

		// multimap parameter and value pairs - like a dictionary
		std::pair<std::string, std::string> entry(file_body_[i].substr(0, pos), file_body_[i].substr(pos + 2));

		param_dictionary_.insert(entry);
	}

}

void reader::printEntries(){

	std::cout << "This is what's in the reader's dictionary:\n\n";

	for (std::multimap<std::string, std::string>::iterator it = param_dictionary_.begin(); it != param_dictionary_.end(); ++it)
	    std::cout << it->first << " => " << it->second << '\n';

}

#endif
