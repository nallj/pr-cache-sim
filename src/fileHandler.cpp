#include "fileHandler.hpp"

// Constructors //

fileHandler::fileHandler() { }

fileHandler::fileHandler(std::string input) :
    input_file_(input) { }

// desired paramters only
fileHandler::fileHandler(std::string input, std::vector<std::string> accept) :
    input_file_(input), acceptable_params_(accept) {

  parseContent(input);
  parseEntries();

  //if (is_valid_) printEntries();

  /*std::string test[] = {"UNKNOWN", "WHITE", "COMMENT", "PARAMETER", "DATA", "INVALID"};
  for (unsigned long i = 0; i < file_body_.size(); i++)
    std::cout << ">  " << file_body_[i].second << "\t\t\t" << test[file_body_[i].first] << std::endl;*/
}

// desired paramters and data body regex
fileHandler::fileHandler(
  std::string input,
  std::vector<std::string> accept,
  std::vector<std::string> regex
) :
  input_file_(input), acceptable_params_(accept), regex_args_(regex) {

  parseContent(input);
  parseEntries();

  //if (is_valid_) printEntries();

  /*std::string test[] = {"UNKNOWN", "WHITE", "COMMENT", "PARAMETER", "DATA", "INVALID"};
  for (unsigned long i = 0; i < 40; i++)
    std::cout << ">  " << file_body_[i].second << "\t\t\t" << test[file_body_[i].first] << std::endl;*/
}

// Operations Functions //

void fileHandler::deleteIfExists() {
  remove(input_file_.c_str());
}

//std::vector<std::string>
void fileHandler::parseContent(std::string fin) {

  file_body_.clear(); // clear out current body

  //std::vector< std::pair<lineType, std::string> > body;
  std::ifstream in_file(fin.c_str());
  std::string line;
  bool comment_block = false;

  while (std::getline(in_file, line)) {

    // handle comment blocks
    if (comment_block || (line.length() > 1 && line.at(0) == LONG_COMMENT_TAG && line.at(1) == COMMENT_TAG) ) {

      // is this the end of a comment block
      if (comment_block && line.length() > 1 && line.at(0) == COMMENT_TAG && line.at(1) == LONG_COMMENT_TAG)
        comment_block = false;

      else
        comment_block = true;

      file_body_.push_back( std::make_pair(COMMENT, line) );
      continue;
    }

    // check for single comment
    if (line.length() > 0 && line.at(0) == COMMENT_TAG) {

      file_body_.push_back( std::make_pair(COMMENT, line) );
      continue;
    }

    // parsing not inhibited by comment block
    if (line.length() == 0)
      file_body_.push_back( std::make_pair(WHITE, line) );

    else{

      // single line comment
      //if (line.length() > 0 && line.at(0) == COMMENT_TAG)
        //continue;

      // push non-disqualified line onto file body holder
      file_body_.push_back( std::make_pair(UNKNOWN, line) );
    }


  }
  in_file.close();

  //return body;
}

void fileHandler::parseEntries() {

  param_dictionary_.clear();

  for (int i = 0; i < file_body_.size(); i++) {

    if (file_body_[i].first == UNKNOWN) {

      std::size_t pos = file_body_[i].second.find(':');

      if (pos != std::string::npos) { // presence of colon implies (but doesn't guarantee) an acceptable parameter

        std::string test_param = file_body_[i].second.substr(0, pos);
        std::string test_arg = file_body_[i].second.substr(pos + 1);

        boost::trim(test_arg);

        if (test_arg.length() == 0) { // check for parameter with no argument

          std::cout << "ERROR: This is not a valid file. <Parameter with No Argument>" << std::endl << std::endl;
          file_body_[i].first = INVALID;

          is_valid_ = false;
          break; // file invalid, stop parsing
        }

        // check if acceptable parameter is present
        if (std::find(acceptable_params_.begin(), acceptable_params_.end(), test_param) != acceptable_params_.end()
            && test_arg.length() != 0) {

          // multimap parameter and value pairs - like a dictionary
          std::pair<std::string, std::string> entry(test_param, test_arg);
          param_dictionary_.insert(entry);

          file_body_[i].first = PARAMETER;
          continue; // parameter accepted, next input
        }

      }


      bool pass_regex = false;
      for (int j = 0; j < regex_args_.size(); j++) {

        boost::smatch match;

        if ( boost::regex_match(file_body_[i].second, match, boost::regex{regex_args_[j]}) ) {

          //data_body_.push_back(file_body_[i]); // insert into data vector
          file_body_[i].first = DATA;

          pass_regex = true;
          break; // regex match found, stop regex parsing
        }
      }

      if (pass_regex) continue; // data accepted, next input


      // getting this far means the line contains neither parameter nor data
      std::cout << "ERROR: This is not a valid file. <Line#" << i << " is Neither Parameter nor Data>" << std::endl << std::endl;
      file_body_[i].first = INVALID;
      is_valid_ = false;

      break;
    }

  } // /for (file body search)

}

void fileHandler::printEntries() {

  std::cout << "> PARAMETER MULTIMAP <\n";
  for (std::multimap<std::string,std::string>::iterator it = param_dictionary_.begin(); it != param_dictionary_.end(); ++it)
    std::cout << "MAP: " << it->first << "  \t=>\t" << it->second << std::endl;

  std::cout << "\n> DATA ENTRIES <" << std::endl;
  for (int i = 0; i < file_body_.size(); i++)
    if (file_body_[i].first == DATA)
        std::cout << "DATA: " << file_body_[i].second << std::endl;

}

void fileHandler::addToFile(std::string line) {

  std::ofstream input_file;
  input_file.open(input_file_.c_str(), std::ios_base::app);

  input_file << std::endl << line;
  input_file.close();
}

std::pair<bool, unsigned long> fileHandler::findInFile(std::string line) {

  for (unsigned long i = 0; i < file_body_.size(); i++)
    if (file_body_[i].second.compare(line) == 0)
      return std::make_pair(true, i);

  return std::make_pair(false, 0);
}

bool fileHandler::removeFromFile(std::string line) {

  std::pair<bool, unsigned long> finding_pair = findInFile(line);

  if (finding_pair.first) { // check for presence of line in file

    std::ofstream fout(input_file_);

    for (unsigned long i = 0; i < file_body_.size(); i++)
          if (i != finding_pair.second)
            fout << file_body_[i].second << std::endl;

    fout.close();
    return true;

  }else std::cout << "\n\nError: Entry not found in file!\n\n";

  return false;
}


// Accessor Methods //

bool fileHandler::isFileValid() { return is_valid_; }

std::multimap<std::string, std::string> fileHandler::getParams() { return param_dictionary_; }

std::vector<std::string> fileHandler::getData() {

  std::vector<std::string> data_body;

  for (unsigned long i = 0; i < file_body_.size(); i++)
    if (file_body_[i].first == DATA)
      data_body.push_back(file_body_[i].second);

  return data_body;
}
