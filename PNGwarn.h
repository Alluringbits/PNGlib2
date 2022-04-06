#pragma once
#include "PNGerrwarnbase.h"

namespace PNGwarn{
	struct noWarn : public PNGewb{
		noWarn() noexcept{};
		const char* what() const noexcept override{
			return "No error or warnings.";
		}
		uint16_t code() const noexcept override{
			return 0;
		}
	};	

}
