#pragma once
#include "PNGmessages/PNGerr.h"
#include <vector>
#include <cstdio>
#include <fstream>
#include <algorithm>

namespace PNG{
	enum flags : uint8_t { Quiet = 1, Soft = 2, Verbose = 4, Short = 8, Long = 16};
	enum anc : int8_t { trns,chrm,gama,iccp,sbit,srgb,time,bkgd,hist,phys,splt,itxt,text,ztxt};

	inline void printHex(const std::vector<unsigned char> & s) noexcept{
		for(size_t i{}; i<s.size(); i++) std::printf("%#x ", s[i]);
		std::printf("\n");
	}

	inline void dumpHex(const char *fn, std::vector<unsigned char> & s) noexcept{
		std::ofstream file_out{fn, std::ios::out};
		file_out.write(reinterpret_cast<char *>(s.data()), s.size());
		file_out.close();
	}

	inline constexpr void printHex(const char * l,size_t s) noexcept{
		for(size_t i{}; i<s; i++) std::printf("%#x ", l[i]);
		std::printf("\n");
	}
	inline constexpr void printHex(const unsigned char * l, size_t s) noexcept{
		for(size_t i{}; i<s; i++) std::printf("%#x ", l[i]);
		std::printf("\n");
	}



	inline void printMsg(const message&  m, bool softMsg = true, bool shortMsg = true) noexcept{
		if(softMsg){
			if(m.code()) (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());	
		}
		else (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());
	}
	inline void printMsgList(std::vector<message>& msgLs) noexcept{
		std::for_each(msgLs.cbegin(),msgLs.cend(), [](const message& m){PNG::printMsg(m, 0,1 );});
	}
	inline void printMsg(const PNGmsgBase & m, bool softMsg = true, bool shortMsg = true) noexcept{
		if(softMsg){
			if(m.code()) (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());	
		}
		else (shortMsg) ? std::printf("%s: %s\n",m.type().data(), m.what().data()) :std::printf("\nMessage Type:  %s\nMessage Code:  %d\nDescription :  %s\n", m.type().data(), m.code(), m.what().data());
	}

}
