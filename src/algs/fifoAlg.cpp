#include "fifoAlg.h"

/* PUBLIC */

fifoAlg::fifoAlg(std::string name, unsigned max_size) :
  replAlg(name, max_size) { }


void fifoAlg::newEntryUpdate(unsigned module_index) {

  // random replacement has no need to assign a weight value to any of it's table records
  tag_table_.push_back( std::make_pair(-1, module_index) );

  replAlg::printTable();
}

unsigned fifoAlg::replace(unsigned module_index) {

  unsigned next_victim = evictNext();
  std::cout << "FIFO REPL: victim is module#" << tag_table_[next_victim].second << " @ table[" << next_victim << "]\n";

  tag_table_.erase(tag_table_.begin() + next_victim);

  newEntryUpdate(module_index);

  return next_victim;
}

/* PRIVATE */

// choose victim from the beginning of the queue (tag table)
unsigned fifoAlg::evictNext() {

  unsigned next_eviction = 0;

  return next_eviction;
}

void fifoAlg::reorderTable() {}
