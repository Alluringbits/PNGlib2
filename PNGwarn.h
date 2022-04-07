#pragma once
#include "PNGerrwarnbase.h"

namespace PNG{
	namespace PNGwarn{
		struct PNGwarnBase : public PNGmsg{
			const char * type() const noexcept override{
				return "Warning";
			}
			virtual uint16_t code() const noexcept = 0;
		};

	}
}
