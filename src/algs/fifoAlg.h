#ifndef _FIFO_ALG_H_
#define _FIFO_ALG_H_

#include "../replAlg.h"


class fifoAlg : public replAlg{

	public:
		fifoAlg(std::string name, unsigned max_size);


		void newEntryUpdate(unsigned module_index);

		unsigned replace(unsigned module_index); // replace a victim with module_index

	private:
		unsigned evictNext(); // choose victim from the beginning of the queue (tag table)

		void reorderTable();

};

#endif
