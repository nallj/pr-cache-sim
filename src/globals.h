#ifndef GLOBALS
#define GLOBALS

enum prcSignal { PRC_MC, PRC_ICAP_REQ, PRC_START_PRR, PRC_ENQUEUE_PRR };
enum icapSignal { ICAP_MC, ICAP_PRC_ACK, ICAP_PRR_REQ, ICAP_TRANSFER_PRR };
enum prrCtrlSignal { PRR_EXE, PRR_PRC_ACK, PRR_ICAP_ACK };		// todo: PRR_PRC_ACK? Do I still need to do this

enum moduleState { VACANT, IDLE, TRANSFER, EXEC };

#endif
