#ifndef FILE_HANDLER
#define FILE_HANDLER

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdio.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#define COMMENT_TAG '#'
#define LONG_COMMENT_TAG '/'

enum lineType { UNKNOWN, WHITE, COMMENT, PARAMETER, DATA, INVALID };

class fileHandler{

  bool is_valid_ = true;

  std::vector<std::string> acceptable_params_;
  std::vector<std::string> regex_args_;
  std::vector< std::pair<lineType, std::string> > file_body_;
  std::multimap<std::string, std::string> param_dictionary_;

public:
  std::string input_file_;

  // Constructors //
  fileHandler();
  fileHandler(std::string input);
  fileHandler(std::string input, std::vector<std::string> accept);
  fileHandler(std::string input, std::vector<std::string> accept, std::vector<std::string> regex);

  // Operations Functions //
  void deleteIfExists();
  void parseContent(std::string fin);
  void parseEntries(); // search for accepted parameters and regex matches for data body
  void printEntries();
  void addToFile(std::string line); // TODO: needs line validation
  std::pair<bool, unsigned long> findInFile(std::string line); // TODO: not finished
  bool removeFromFile(std::string line); // TODO: not finished

  // Accessor Methods //
  bool isFileValid();
  std::multimap<std::string, std::string> getParams();
  std::vector<std::string> getData();
};

#endif
