#pragma once
#include "PNGerrwarnbase.h"

namespace PNG{
	/*namespace PNGwarn{
		struct PNGbasicWarn : public PNGmsgBase{
			const char * type() const noexcept override{
				return "\033[1;35mWarning\033[0m";
			}
			virtual uint16_t code() const noexcept = 0;
		};
		struct testWarn : public PNGbasicWarn{
			testWarn() noexcept{ val = "This is a Warning test Message.";};
			uint16_t code() const noexcept{return 0;};
		};
	}*/
	struct PNGwarning : public PNGmsgBase{
		PNGwarning(std::string l) noexcept{ val  = l;};
		const std::string_view type() const noexcept override{
			return tp;
		}
		uint16_t code() const noexcept { return 0;};
		operator bool() const noexcept override{
			return false;
		}
		private:
			std::string_view tp{"\033[1;35mWarning\033[0m"};
	};
}
