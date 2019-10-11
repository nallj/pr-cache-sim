#ifndef _REPL_ALG_
#define _REPL_ALG_

#include <iostream>
#include <string>
#include <vector>
#include <utility>

class replAlg{
protected:
	std::vector< std::pair<int, unsigned> > tag_table_;
	unsigned max_table_size_;

	replAlg(std::string name, unsigned max_size);

	virtual unsigned evictNext() = 0; // find location of next table entry to evict (NOT the module index)
	virtual void reorderTable() = 0;

public:
	std::string name_;

	//virtual void readHitUpdate(unsigned module_index) = 0;
	//virtual void replaceUpdate(unsigned module_index) = 0;
	//virtual void updateTable(unsigned module_index) = 0;

	virtual void newEntryUpdate(unsigned module_index) = 0;

	virtual unsigned replace(unsigned module_index) = 0;
		// replace next eviction with module_index, return table index of victim (NOT the module index)

	// DEBUG FUNCTIONS //
	virtual void printName();
	virtual void printTable();
};

#endif
