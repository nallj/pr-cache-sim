#include "replAlg.hpp"

/* PUBLIC */

// DEBUG FUNCTIONS //
void replAlg::printName() {
  std::cout << "\t\t\t\tReplacement: '" << name_ << "'\n";
}

void replAlg::printTable() {
  std::cout << "\tPrinting tag table of \"" << name_ << "\".\n";

  for (unsigned i = 0; i < tag_table_.size(); i++)
    std::cout << "\t\t" << i << "\t" << tag_table_[i].second << " <" << tag_table_[i].first << ">\n";
}

/* PROTECTED */

replAlg::replAlg(std::string name, unsigned max_size) :
  name_(name), max_table_size_(max_size) { }
