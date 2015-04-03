#ifndef _REPL_ALG_H_
#define _REPL_ALG_H_

#include <string>
#include <vector>
#include <utility>

class replAlg{

	public:
		replAlg(std::string name);

		void readHitUpdate(unsigned module_index);

		void newEntryUpdate(unsigned module_index);

		unsigned evictNext();

		std::string name_;

	private:
		void reorderTable();

		std::vector< std::pair<int, unsigned> > tag_table_;
};

#endif
