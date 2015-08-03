#ifndef _RANDOM_ALG_
#define _RANDOM_ALG_

#include "randomAlg.h"


/* PUBLIC */

	randomAlg::randomAlg(std::string name, unsigned max_size) :
			replAlg(name, max_size){ }


	void randomAlg::newEntryUpdate(unsigned module_index){

		// random replacement has no need to assign a weight value to any of it's table records
		tag_table_.push_back( std::make_pair(-1, module_index) );

		replAlg::printTable();
	}

	unsigned randomAlg::replace(unsigned module_index){

		unsigned next_victim = evictNext();
		std::cout << "RAND REPL: victim is module#" << tag_table_[next_victim].second << " @ table[" << next_victim << "]\n";

		tag_table_.erase(tag_table_.begin() + next_victim);

		newEntryUpdate(module_index);

		return next_victim;
	}


/* PRIVATE */

	// constantly check for the true randomness of this alg
	unsigned randomAlg::evictNext(){

		unsigned next_eviction;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<unsigned> dist(0, tag_table_.size() - 1);

		next_eviction = dist(mt);

		return next_eviction;
	}

	void randomAlg::reorderTable(){}

#endif
