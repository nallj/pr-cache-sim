#ifndef _RANDOM_ALG_H_
#define _RANDOM_ALG_H_

//#include <ctime>
#include <random>

#include "../replAlg.h"


class randomAlg : public replAlg{

	public:
		randomAlg(std::string name, unsigned max_size);


		void readHitUpdate(unsigned module_index); // with random replacement, nothing occurs with a read hit

		void newEntryUpdate(unsigned module_index);

		unsigned replace(unsigned module_index); // randomly replace a victim with module_index

	private:
		unsigned evictNext(); // randomly choose a victim in the tag table

		void reorderTable();

};

#endif
