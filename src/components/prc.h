#ifndef __PRC_H__
#define __PRC_H__

#include <iostream>
#include <vector>
#include <deque>

#include "../globals.h"
#include "../traceToken.h"
#include "../storage/memoryLevel.h"
#include "../storage/reconfigurableRegions.h"
#include "prrLevelController.h"

enum prcState {
	PRC_INIT,
	PRC_LATCH,
	PRC_FIND_LOOP,
	PRR_START,
	PRR_ENQUEUE,
	PRC_REQ_ICAP,
	CACHE_SEARCH,
	SEARCH_WAIT
};

class prc {

	double prc_speed_;
	std::vector<bool> has_memory_lookup_table;
	unsigned prc_counter_, memory_counter_, trace_counter_;

	// internal signals
	prcState current_state_, next_state_;
	unsigned stall_count_;

	// IN signals
	bool *mem_search_done_, *mem_search_found_;
	std::vector<prrControllerState*> prr_ctrl_status_;
	std::deque<bool>* prr_executing_;

	std::vector<storageUnit*> *memory_hierarchy_;
	reconfigurableRegions *memory_hierarchy_top_;

	// OUT signals
	bool prr_ctrl_execute_, memory_ack_, icap_req_;
	std::deque<bool> prr_enqueue_;
	std::deque<bool> prr_start_;
	bool *icap_ack_, *icap_trans_, *prr_ack_;

	// IN/OUT signal
	std::vector<traceToken*>* traces_;
	traceToken* current_trace_;

	traceToken** icap_current_trace_ptr_;

public:
	prc(double prc_speed);
	~prc();

	void connect(
		std::vector<storageUnit*>* memory_hierarchy,
		reconfigurableRegions* memory_hierarchy_top,
		std::vector<traceToken*>* traces,
		std::deque<bool>* prr_executing,
		bool* prr_ack,
		bool* icap_ack,
		bool *icap_trans,
		traceToken** icap_current_trace_ptr);

	void step();


	// OUT signals

	// dont change this - this is good
	traceToken** accessTrace();

	// PRC_MC
	// getter function used by the signal context to retrieve signals
	unsigned* accessCounterSignal(prcSignal signal);

	// PRC_ICAP_REQ
	// getter function used by the signal context to retrieve signals
	bool* accessSignal(prcSignal signal);

	// PRC_ENQUEUE_PRR, PRC_START_PRR
	std::deque<bool>* accessSignalBus(prcSignal signal);
};

#endif
