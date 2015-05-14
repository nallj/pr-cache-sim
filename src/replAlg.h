#ifndef _REPL_ALG_
#define _REPL_ALG_

#include <iostream>
#include <string>
#include <vector>
#include <utility>

//#include "storageUnit.h"

class replAlg{

	public:
		virtual void readHitUpdate(unsigned module_index) = 0;

		virtual void newEntryUpdate(unsigned module_index) = 0;

		virtual unsigned replace(unsigned module_index) = 0;
			// replace next eviction with module_index, return table index of victim

		std::string name_;

		// DEBUG FUNCTIONS //
			virtual void printName(){ std::cout << "Replacement algorithm is known as \"" << name_ << "\".\n"; }

			virtual void printTable(){

				std::cout << "Printing tag table of \"" << name_ << "\".\n";

				for(unsigned i = 0; i < tag_table_.size(); i++)
					std::cout << "\t" << i << "\t" << tag_table_[i].second << " <" << tag_table_[i].first << ">\n";
			}


	protected:
		replAlg(std::string name, unsigned max_size) :
			name_(name), max_table_size_(max_size){ }


		virtual unsigned evictNext() = 0; // find location of next entry in table to evict (NOT the module index)

		virtual void reorderTable() = 0;

		std::vector< std::pair<int, unsigned> > tag_table_;

		unsigned max_table_size_;
};

#endif
