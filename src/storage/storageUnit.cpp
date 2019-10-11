#include "storageUnit.h"

/* PUBLIC */

storageUnit::storageUnit() :
	unit_size_(0), read_latency_(0), search_latency_(0), child_unit_(NULL) { }

storageUnit::storageUnit(
	unsigned unit_size,
	unsigned short read_latency,
	unsigned short search_latency
) :
	unit_size_(unit_size),
	read_latency_(read_latency),
	search_latency_(search_latency),
	child_unit_(NULL) { }

storageUnit::storageUnit(const storageUnit& su) {
	*this = su;
}

// Accessors //

unsigned short storageUnit::getSize() {
	return unit_size_;
}

memoryLevel* storageUnit::getChild() {
	return child_unit_;
}

unsigned short int storageUnit::getReadLatency() {
	return read_latency_;
}

unsigned short int storageUnit::getSearchLatency() {
	return search_latency_;
}

// Mutators //

void storageUnit::setChild(memoryLevel* child_level) {
	child_unit_ = child_level;
}
