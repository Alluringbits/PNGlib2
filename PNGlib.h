#pragma once

//second library guard is unnecessary but..
#ifndef ___PNGLIB__
#define ___PNGLIB__

#include <iostream>
#include <cstdint>
#include <exception>
#include <zlib.h>
#include <string_view>
#include <cstring>
#include <vector>
#include <initializer_list>
#include <memory>
#include <algorithm>

//Messages class system
#include "PNGerrwarnbase.h"
#include "PNGwarn.h"
#include "PNGmsg.h"
#include "PNGerr.h"

#ifndef __PNGEXC
	#undef PNGEXC 
	#define PNGEXC 
#else
	#define PNGEXC 
#endif


namespace PNG{
	extern bool PNGTRW;

	class basic_PNG{
		public:
			basic_PNG() noexcept : fn{}, bPNGios{nullptr}{};
			basic_PNG(const std::string_view filename, const std::string_view flags = "ss") noexcept : fn{filename}, bPNGios{nullptr}{
				livePrint(flags);
				for(size_t i{}; i<anChunksNum;i++){
					anChunks[i].reset();
					anChunks[i].name = ancID[i];
				}
			};
			virtual const std::unique_ptr<PNGmsgBase>& open(std::string_view) PNGEXC = 0;
			virtual const std::unique_ptr<PNGmsgBase>& close() = 0;
			constexpr const std::string_view  fileName() const noexcept{return fn;};

			//std::basic_istream and std::basic_ostream have the copy ctors deleted, since the file interface is done through that, only the move ctors will be declared. A separated function can copy the data contents from one object PNG to another object PNG
			basic_PNG& operator=(const basic_PNG& t) = delete;
			basic_PNG(const basic_PNG& t) = delete;
			//move ctors for consistency and usefulness
			basic_PNG(basic_PNG && t) noexcept = default;
			basic_PNG& operator=(basic_PNG && t) noexcept = default;
			virtual ~basic_PNG() noexcept {bPNGios = nullptr;};//FOr safety sets to nullptr the std::basic_ios<char> ptr
			
			std::ios_base::iostate fsstate() noexcept{
				return bPNGios->rdstate(); //WILL cause segmentation fault if bPNGios is NOT initialized to point to a fstream object - this is expected in implementation of std::basic_ios, should there be an exception throw?
			}	



			//Messages vector functions: get last message in the messages list, get the entire message list, empty the messages list, print entire messages list
			
			void emptyMsgList() noexcept{
				msgLs.clear();
				msgLs.push_back(std::make_unique<noMsg>());
			}
			
			const std::unique_ptr<PNGmsgBase>& lastMsg() noexcept{
				//if(msgLs.empty()) msgLs.push_back(std::make_unique<noMsg>()); //consequence of using std::unique_ptr, unless I'm missing something, is that an already initialized neutral message (neutralMsg) can be pushed back to the to the vector of unique ptrs to the messages. It must be constructed in place with make_unique
				if(msgLs.empty()) return neutralMsg;
				return msgLs.front();
			}
			
			const std::vector<std::unique_ptr<PNGmsgBase>>& msgList() noexcept{
				return msgLs;
			}
			
			void printMsgList() const noexcept{
				std::for_each(msgLs.cbegin(),msgLs.cend(), [](const std::unique_ptr<PNGmsgBase>& m){printMsg(m, 0,1 );});
			}


			void livePrint(const std::string_view flags = "ss") noexcept{
				if(flags[0] == 's'){
					softMsg = true;
					quietMsg = false;
				}
				else if(flags[0] == 'q'){
					softMsg= false;
					quietMsg = true;
				}
				else{
					softMsg = false;
					quietMsg = false;
				}
				(flags[1] == 's') ? (shortMsg = true) : (shortMsg = false);
			}

			
			void reset() noexcept; //empties out everything, resets every byte and zeroes every char array

			

			void test(){
				for(size_t i{}; i<=fn.size(); i++){
				//	std::printf("%d\n", static_cast<int>(fn[i]));
				}
				//std::cout << (anChunks[3].data) << "\n";
			}	
				
		protected:
				
			//types declarations string_view and chunk_t
			//using sv=std::string_view; //
			class chunk_t {//usage of chunk_t::data requires first to resize it to the chunk_t::size value, then the chunk_t::data::data() can be used to return the pointer for the read(*,n) func
				public:
				chunk_t() noexcept: size{}, pos{}, name{}, data{}, crc{}, exist{}{};
				//constexpr chunkStruct_t(uint32_t s, std::initializer_list<uint8_t> n, uint8_t * d, std::initializer_list<uint8_t>  c) : size{s}, data{d}{std::copy(n.begin(), n.end(), name); std::copy(c.begin(), c.end(), crc);}; //failed std::initializer_list ctor
				chunk_t(uint32_t s, uint32_t p, std::string_view n, std::initializer_list<unsigned char> d, uint32_t c, bool e) : size{s}, pos{p}, name{n}, data{d}, crc{c}, exist{e}{data.shrink_to_fit();};
			
				chunk_t(const chunk_t & t) noexcept = default;
				
				//deprecated, usage of C-array for data
				//constexpr chunk_t(const chunk_t & t) noexcept : size{t.size}, pos{t.pos}, name{t.name}, data{static_cast<uint8_t *>(std::calloc(size, sizeof(uint8_t)))}, crc{t.crc}{std::strncpy(static_cast<char *>(data), static_cast<char *>(t.data), size);}; //this one works equivalently, no assurance on memory security with the 
				chunk_t& operator=(const chunk_t & t) noexcept = default;
				
				//move ctors, for consistency
				chunk_t(chunk_t && t) noexcept = default;
				chunk_t& operator=(chunk_t && t) noexcept = default;
				~chunk_t() noexcept{};
				
				void reset() noexcept{
					size = 0;
					pos = 0;
					data.clear();
					data.shrink_to_fit();
					crc = 0;
					exist = false;
				}	

				uint32_t size; 
				std::streamoff pos; //position in the file of the beginning of the chunk, starting from the 4 bytes long chunk size sequence. e.g. pos is ALWAYS 8 (sigSize) for IHDR and it's NEVER 0 for ANY chunk - will be used for file health checking operations
				std::string_view name; //std::string_view better for optimizations and faster to deal with, especially for safe constructor of the chunk_t and destruction compared to static array of unsigned chars
				
				std::vector<unsigned char> data; //due to  memory allocation issues it may be better to use std::vector rather than raw pointers
				uint32_t crc;


				bool exist;

			};
			
			
			//enums of internal chunk constants, ancillary chinks order indexes, IHDR bits array indexes
			enum constants : int8_t	{infoSize=4, ihdrBitsNum=5, sigSize=8,ihdrSize=13, anChunksNum}; 
			enum critical : int8_t 	{ihdr, plte, idat, iend};
			enum ancillary : int8_t {tRNS, cHRM, gAMA, iCCP, sBIT, sRGB, iTXt, tEXt, zTXt, bKGD, hIST, pHYs, sPLT, tIME};
			enum ihdrbits : int8_t	{depth=12, color, compression, filter, interlace};

			//std::string_view for critical and ancillary chunks identification names
			static constexpr std::string_view critID[infoSize]{"IHDR", "PLTE", "IDAT", "IEND"};
			static constexpr std::string_view ancID[anChunksNum]{"tRNS", "cHRM", "gAMA", "iCCP", "sBIT", "sRGB", "iTXt", "zTXt", "bKGD", "hIST", "pHYs", "sPLT", "tIME", "tEXt"};
			
			//width, height, [bit depth, color scheme, compression method, filter type, interlacing method], these are all included in the IHDR chunk
			uint32_t width{}, height{};
			
		

			//Critical Chunks chunk_t variables
			static constexpr unsigned char signature[]{137,80,78,71,13,10,26,10}; //this is technically not a chunk
			chunk_t IHDR{ihdrSize,sigSize, critID[ihdr], {}, 0, true};
			chunk_t PLTE{0,0,critID[plte], {}, 0, false}; //though it's not necessary if the palette color bit is not set, the PNG standard classifies it as critical - conformity
			//chunk_t IDAT{0,0,critID[idat], {}, 0, true};
			std::vector<chunk_t> IDAT{};
			//static constexpr chunk_t IEND{0, {0x49, 0x45, 0x4E, 0x44}, nullptr, {0xAE, 0x42, 0x60, 0x82}}; std::initializer_list needed for custom struct ctor- how?
			//static constexpr chunk_t IEND{0, critID[iend] , nullptr, "\xAE\x42\x60\x82"}; failed static constexpr declaration of IEND with chunk_t type  - weird g++ error
			const chunk_t IEND{0, 0,  critID[iend] , {}, 2577752367, true}; //chunk_t for conformity	
			//chunk_t array of ancillary chunks
			chunk_t anChunks[anChunksNum];
			
			
			
			//file interface variables
			std::string_view fn;
			std::basic_ios<char> *bPNGios;
			basic_PNG(std::string_view f, std::basic_ios<char> *pbs, std::string_view flags = "ss") noexcept : fn{f}, bPNGios{pbs}{
				livePrint(flags);
				//Stupid and bad, since it only has 2 uses might as well not make it an internal function of chunk_t
				for(size_t i{}; i<anChunksNum;i++){
					anChunks[i].reset();
					anChunks[i].name = ancID[i];
				}
			};
 //protected constructor for the derived classes
			//bool wasInitialized; //could be implemented in the future to throw an exception when doing ANYTHING in the base class that's dependent on the fstream when the fstream has NOT been implemented yet - might be ok to check bPNGios as boolean?
			

			//error vector list - after some consideration there will be no handling of errors in a std::queue system with popping of first error after it has been requested by the client - might be worth looking into it in the future for a possible efficient and functional impl
			std::vector<std::unique_ptr<PNGmsgBase>> msgLs{};//memorandum: change and replace unique_ptrs with actual normal PNGmsgBase objects, they may not even be needed as the objects would be returned by the functions as const references to the vector member, rendeding smart pointers and assurance it gets deleted completely useless as the deletion is assured by the vector destructor and destructor of PNGmsgBase itself. note: it may be that returning a non pointer address to a base class with a derived class is not feasible through the usage of addresses, the only way would be by returning a const pointer to the element rather than the address
			std::unique_ptr<PNGmsgBase> neutralMsg{std::make_unique<noMsg>()};	
			template<typename T>
			const std::unique_ptr<PNGmsgBase>&  pngraise(T && e){
				if(PNGTRW) throw(e);
				else{
					msgLs.push_back(std::make_unique<T>(std::forward<T>(e)));
					if(!(quietMsg)) printMsg(msgLs.back(), softMsg, shortMsg);
					return msgLs.back();
				}
			}
			template<typename T>
			void pnglog(T && e){
				msgLs.push_back(std::make_unique<T>(std::forward<T>(e)));
				if(!(quietMsg)) printMsg(msgLs.back(), softMsg, shortMsg);
			}
			char * rcp(unsigned char * t) const {
				return reinterpret_cast<char *>(t);
			}
			bool softMsg{true}, quietMsg{false}, shortMsg{true}, isClosed{false};
			
			uint64_t base_crc{crc32(0L, NULL, 0)};

			constexpr uint32_t btoi(const char * l) const noexcept{
				return 	(l[0] << 24) | (l[1] << 16) | (l[2] << 8) | l[3];
			}	
			constexpr uint32_t btoi(const unsigned char * l) const noexcept{
				return 	(l[0] << 24) | (l[1] << 16) | (l[2] << 8) | l[3];
			}	
			constexpr char * itob(const uint32_t l) const noexcept{
				char * val{new char[4]};
				for(int i{3}; i>=0; i--) val[3-i] = static_cast<char>((l >> (8*i) & 255));
				return val;
			}	
			constexpr void printHex(const char * l,size_t s) const noexcept{
				for(size_t i{}; i<s; i++) std::printf("%#x ", l[i]);
				std::printf("\n");
			}
	};
}

#endif
