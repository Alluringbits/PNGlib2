#pragma once
#include <exception>
#include <cstdint>
#include <string>
#include <cstdio>
#include <memory>

namespace PNG{
	struct PNGmsgBase : public std::exception{
		const char * what() const noexcept override {
			return val.data();
		}
		virtual const char * type() const noexcept = 0;
		virtual uint16_t code() const noexcept = 0;
		virtual operator bool() const noexcept = 0;
		protected:
			std::string val{};
	};
	struct noMsg : public PNGmsgBase{
		noMsg() noexcept{ val = "No errors were thrown by this funciton.";};
		const char* type() const noexcept override{
			return "None";
		}
		uint16_t code() const noexcept override{
			return 0;
		}
		operator bool() const noexcept override{
			return false;
		}
	};
	
	inline void printMsg(const std::unique_ptr<PNGmsgBase>&  m, bool softMsg = true, bool shortMsg = true) noexcept{
		if(softMsg){
			if(!(m->code())) (shortMsg) ? std::printf("%s: %s\n",m->type(), m->what()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m->type(), m->code(), m->what());	
		}
		else (shortMsg) ? std::printf("%s: %s\n",m->type(), m->what()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m->type(), m->code(), m->what());
	}
	inline void printMsg(const PNGmsgBase & m, bool softMsg = true, bool shortMsg = true) noexcept{
		if(softMsg){
			if(!(m.code())) (shortMsg) ? std::printf("%s: %s\n",m.type(), m.what()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type(), m.code(), m.what());	
		}
		else (shortMsg) ? std::printf("%s: %s\n",m.type(), m.what()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type(), m.code(), m.what());
	}
}
