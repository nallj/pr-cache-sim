#ifndef _MODULE_
#define _MODULE_

#include "module.h"


/* PUBLIC */
	module::module(unsigned id, unsigned bitstream_width) :
			module_id_(id), bitstream_width_(bitstream_width){}


	unsigned module::getId(){
		return module_id_;
	}

	void module::reasignId(unsigned new_id){
		module_id_ = new_id;
	}

	unsigned module::getSize(){
		return bitstream_width_;
	}


/* PRIVATE */
	unsigned module_id_;

	unsigned bitstream_width_;


#endif
