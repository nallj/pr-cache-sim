#ifndef PRR_LEVEL_CONTROLLER
#define PRR_LEVEL_CONTROLLER

#include <string>
#include <vector>
#include <tuple>
#include <queue>
#include <utility>

#include "../globals.h"
#include "../storage/reconfigurableRegions.h"
#include "../storage/memoryLevel.h"
#include "../traceToken.h"


enum prrControllerState { PRR_INIT, PRR_IDLE, PRR_WAIT, PRR_TRANSFER };

class prrLevelController {

	prrControllerState current_state_, next_state_;

	reconfigurableRegions* prr_;
	std::vector<storageUnit*> *memory_hierarchy_;

	unsigned prr_id_;

	double sim_speed_;


	// IN signals
	bool *prc_start_, *prc_enqueue_, *icap_req_, *icap_trans_;

	traceToken** prc_current_trace_ptr_;
	traceToken* prc_current_trace_;

	unsigned* icap_mc_;
	unsigned memory_counter_;
	traceToken** icap_current_trace_ptr_;
	traceToken* icap_current_trace_;

	// OUT signals
	bool prr_executing_, icap_ack_, prc_ack_;

	// INTERNAL
	std::queue<traceToken*> action_queue_;

public:
	prrLevelController(
		unsigned prr_id,
		reconfigurableRegions* memory_hierarchy_top,
		std::vector<storageUnit*>* memory_hierarchy,
		double sim_speed
	);

	~prrLevelController();

	void connect(
		bool *prc_start,
		bool *prc_enqueue,
		unsigned *icap_mc,
		bool *icap_req,
		bool *icap_trans,
		traceToken** prc_current_trace_ptr,
		traceToken** icap_current_trace_ptr
	);

	void step();

	// PRR_EXE, PRR_PRC_ACK, PRR_ICAP_ACK
	bool* accessSignal(prrCtrlSignal signal);
};

#endif
