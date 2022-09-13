#pragma once
#include <exception>
#include <cstdint>
#include <string>
#include <cstdio>
#include <memory>
#include <string_view>

namespace PNG{

	struct PNGmsgBase{
		const std::string& what() const noexcept { 
			return val;
		}
		virtual const std::string_view type() const noexcept = 0;
		virtual uint16_t code() const noexcept = 0;
		virtual operator bool() const noexcept = 0;
		protected:
			std::string val{};
	};
	
	struct message{
		template<typename T>
		message(T a) noexcept : msg{std::make_unique<T>(a)}{};
		const std::string_view type() const noexcept{return msg->type();};
		uint16_t code() const noexcept {msg->code();};
		const std::string& what() const noexcept{msg->what();};
		operator bool() const noexcept{return bool(*(msg));};

		private:
			std::unique_ptr<PNGmsgBase> msg;
	};

	struct noMsg : public PNGmsgBase{
		noMsg() noexcept{ val = "No errors were thrown by this funciton.";};
		const std::string_view type() const noexcept override{
			return tp;
		}
		uint16_t code() const noexcept override{
			return 0;
		}
		operator bool() const noexcept override{
			return false;
		}
		private:
			std::string_view tp{"None"};
	};
	
	inline void printMsg(const message&  m, bool softMsg = true, bool shortMsg = true) noexcept{
		if(softMsg){
			if(!(m.code())) (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());	
		}
		else (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());
	}
	inline void printMsg(const PNGmsgBase & m, bool softMsg = true, bool shortMsg = true) noexcept{
		if(softMsg){
			if(!(m.code())) (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());	
		}
		else (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());
	}
}
