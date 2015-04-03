#ifndef _MODULE_H_
#define _MODULE_H_

#include <string>

class module{

	public:
		module(unsigned id, unsigned bitstream_width);

		unsigned getId();

		unsigned getSize();

	private:
		unsigned module_id_;

		unsigned bitstream_width_;
};

#endif
