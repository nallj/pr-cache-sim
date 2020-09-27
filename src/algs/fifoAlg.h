#ifndef FIFO_ALG
#define FIFO_ALG

#include "replAlg.h"

class fifoAlg : public replAlg {

  unsigned evictNext(); // choose victim from the beginning of the queue (tag table)
  void reorderTable();

public:
  fifoAlg(std::string name, unsigned max_size);

  void newEntryUpdate(unsigned module_index);
  unsigned replace(unsigned module_index); // replace a victim with module_index
};

#endif
