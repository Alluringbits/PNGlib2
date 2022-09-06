#pragma once
#include "PNGerrwarnbase.h"


namespace PNG{
	/*namespace PNGmsg{
		struct PNGbasicMsg : public PNGmsgBase{
			const char * type() const noexcept{
				return "\033[1;36mMessage\033[0m";
			};
			virtual uint16_t code() const noexcept = 0;
		};
		
		struct testMsg : public PNGbasicMsg{
			testMsg() noexcept { val = "This is a Normal test Message.";};
			uint16_t code() const noexcept{return 0;};
		};
		struct file_opening : public PNGbasicMsg{
			file_opening(const char* l) noexcept { val = std::string("Opening file \"") + l + "\".";};
			uint16_t code() const noexcept{return 200;};
		};
		struct file_closing : public PNGbasicMsg{
			file_closing(const char* l) noexcept { val = std::string("Closing file \"") + l  + "\".";};
			uint16_t code() const noexcept{return 201;};
		};
	}*/
	struct PNGmessage : public PNGmsgBase{
		PNGmessage(std::string l) noexcept { val = l;};
		const std::string_view type() const noexcept{
			return tp;
		};
		uint16_t code() const noexcept {return 0;};
		operator bool() const noexcept override{
			return false;
		}
		private:
			std::string_view tp{"\033[1;36mMessage\033[0m"};
	};

}
