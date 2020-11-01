#ifndef NALLJ_DRACHMA_FIFO_ALG
#define NALLJ_DRACHMA_FIFO_ALG

#include "replAlg.hpp"

class fifoAlg : public replAlg {

  unsigned evictNext(); // choose victim from the beginning of the queue (tag table)
  void reorderTable();

public:
  fifoAlg(std::string name, unsigned max_size);

  void newEntryUpdate(unsigned module_index);
  unsigned replace(unsigned module_index); // replace a victim with module_index
};

#endif
