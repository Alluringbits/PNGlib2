#pragma once
#include <exception>
#include <variant>
#include <cstdint>
#include <string>
#include <cstdio>
#include <memory>

namespace PNG{
	struct PNGmsg : public std::exception{
		const char * what() const noexcept override {
			return val.data();
		}
		virtual const char * type() const noexcept = 0;
		virtual uint16_t code() const noexcept = 0;
		protected:
			std::string val{};
	};
	struct noMsg : public PNGmsg{
		noMsg() noexcept{ val += "No error, warnings or messages.";};
		const char* type() const noexcept override{
			return "None";
		}
		uint16_t code() const noexcept override{
			return 0;
		}
	};
	inline void printMsg(const std::unique_ptr<PNGmsg>&  m) noexcept{
		std::printf("Message Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m->type(), m->code(), m->what());	
	}
}
