#pragma once

#include "PNGincludes.h"
#include <fstream>

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
				initanC();
			};
			virtual const message& open(std::string_view) PNGEXC = 0;
			virtual const message& close() = 0;
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
				msgLs.push_back(noMsg());
			}
			
			const message& lastMsg() noexcept{
				//if(msgLs.empty()) msgLs.push_back(std::make_unique<noMsg>()); //consequence of using std::unique_ptr, unless I'm missing something, is that an already initialized neutral message (neutralMsg) can be pushed back to the to the vector of unique ptrs to the messages. It must be constructed in place with make_unique
				if(msgLs.empty()) return neutralMsg;
				return msgLs.front();
			}
			
			const std::vector<message>& msgList() noexcept{
				return msgLs;
			}
			
			void printMsgList() const noexcept{
				std::for_each(msgLs.cbegin(),msgLs.cend(), [](const message& m){printMsg(m, 0,1 );});
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
				chunk_t() noexcept: size{}, pos{}, name{}, data{}, data2{}, crc{}, exist{}{data.resize(0);};
				//constexpr chunkStruct_t(uint32_t s, std::initializer_list<uint8_t> n, uint8_t * d, std::initializer_list<uint8_t>  c) : size{s}, data{d}{std::copy(n.begin(), n.end(), name); std::copy(c.begin(), c.end(), crc);}; //failed std::initializer_list ctor
				chunk_t(uint32_t s, uint32_t p, std::string_view n, std::initializer_list<unsigned char> d, uint32_t c, bool e) : size{s}, pos{p}, name{n}, data{d}, data2{}, crc{c}, exist{e}{data.shrink_to_fit();};
			
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
					if(double * pval = std::get_if<double>(&data2)) *pval = 0;
					else{ std::get<0>(data2).clear(); std::get<0>(data2).shrink_to_fit();}
					data.shrink_to_fit();
					crc = 0;
					exist = false;
				}	

				uint32_t size; 
				std::streamoff pos; //position in the file of the beginning of the chunk, starting from the 4 bytes long chunk size sequence. e.g. pos is ALWAYS 8 (sigSize) for IHDR and it's NEVER 0 for ANY chunk - will be used for file health checking operations
				std::string_view name; //std::string_view better for optimizations and faster to deal with, especially for safe constructor of the chunk_t and destruction compared to static array of unsigned chars
				
				std::vector<unsigned char> data; //due to  memory allocation issues it may be better to use std::vector rather than raw pointers
				std::variant<std::vector<unsigned char>, double> data2; //secondary optional storage for the chunk e.g. un/compressed data etc
				uint32_t crc;


				bool exist;

			};
			
			
			//enums of internal chunk constants, ancillary chinks order indexes, IHDR bits array indexes
			enum constants : int8_t	{infoSize=4, ihdrBitsNum=5, sigSize=8,ihdrSize=13}; 
			enum anConsts : int8_t {anChunksMNum = 4, anChunksSNum = 10, anChunksNum = 14};
			enum critical : int8_t 	{ihdr, plte, idat, iend};
			enum ancillary : int8_t {tRNS, cHRM, gAMA, iCCP, sBIT, sRGB, tIME, bKGD, hIST, pHYs, sPLT, iTXt, tEXt, zTXt};
			enum ihdrbits : int8_t	{depth=8, color, compression, filter, interlace};

			//std::string_view for critical and ancillary chunks identification names
			static constexpr std::string_view critID[4]{"IHDR", "PLTE", "IDAT", "IEND"};
			static constexpr std::string_view ancID[anChunksNum]{"tRNS", "cHRM", "gAMA", "iCCP", "sBIT", "sRGB", "tIME", "bKGD", "hIST", "pHYs", "sPLT", "iTXt", "tEXt",  "zTXt"};
			//crc32 values for each chunk name, saves time in crc32 calculation
			static constexpr uint32_t critCRC[4]{0xa8a1ae0a, 0x4ba88955, 0x35af061e, 0xae426082};
			static constexpr uint32_t ancCRC[anChunksNum]{0x36b970cc, 0xf6be5972, 0xb2e1b71f, 0xa589193e, 0x666de861, 0x101cd3ce, 0xf933dacf, 0x22ddefe, 0x5d0d2dc5, 0x96876563, 0x4b86b3a, 0x12bba348, 0x9642c585, 0x6b797bf1};
			
			//width, height, [bit depth, color scheme, compression method, filter type, interlacing method], these are all included in the IHDR chunk
			uint32_t width{}, height{};
			
		

			//Critical Chunks chunk_t variables
			//static constexpr unsigned char signature[]{137,80,78,71,13,10,26,10}; //this is technically not a chunk
			static constexpr std::string_view signature{"\x89\x50\x4e\x47\xd\xa\x1a\xa"}; //this is technically not a chunk
			chunk_t IHDR{ihdrSize,sigSize, critID[ihdr], {},  0, false};
			size_t  pixelBytes{};
			float bitsMult{1};
			chunk_t PLTE{0,0,critID[plte], {},0, false}; //though it's not necessary if the palette color bit is not set, the PNG standard classifies it as critical - conformity
			//chunk_t IDAT{0,0,critID[idat], {}, 0, true};
			std::vector<chunk_t> IDAT{};
			//static constexpr chunk_t IEND{0, {0x49, 0x45, 0x4E, 0x44}, nullptr, {0xAE, 0x42, 0x60, 0x82}}; std::initializer_list needed for custom struct ctor- how?
			//static constexpr chunk_t IEND{0, critID[iend] , nullptr, "\xAE\x42\x60\x82"}; failed static constexpr declaration of IEND with chunk_t type  - weird g++ error
			chunk_t IEND{0, 0,  critID[iend] ,{}, 2577752367, false}; //chunk_t for conformity	
			//chunk_t array of ancillary chunks
			//chunk_t anChunks[anChunksNum];
			
			chunk_t anChunksS[anChunksSNum];
			std::vector<chunk_t> anChunksM[anChunksMNum];

			std::vector<unsigned char> compressedData{};
			std::vector<unsigned char> decompressedData{};
			std::vector<unsigned char> rawPixelData{};
			
			
			void initanC(){
				for(size_t i{}; i<anChunksSNum; i++){
					anChunksS[i].reset();
					anChunksS[i].name = ancID[i];
				}
				for(size_t i{}; i<anChunksMNum; i++){
					anChunksM[i].clear();
				}
			}	
			
			//file interface variables
			std::string_view fn;
			std::basic_ios<char> *bPNGios;
			basic_PNG(std::string_view f, std::basic_ios<char> *pbs, std::string_view flags = "ss") noexcept : fn{f}, bPNGios{pbs}{
				livePrint(flags);
				initanC();
			};
 //protected constructor for the derived classes
			//bool wasInitialized; //could be implemented in the future to throw an exception when doing ANYTHING in the base class that's dependent on the fstream when the fstream has NOT been implemented yet - might be ok to check bPNGios as boolean?
			

			//error vector list - after some consideration there will be no handling of errors in a std::queue system with popping of first error after it has been requested by the client - might be worth looking into it in the future for a possible efficient and functional impl
			std::vector<message> msgLs{};//memorandum: change and replace unique_ptrs with actual normal PNGmsgBase objects, they may not even be needed as the objects would be returned by the functions as const references to the vector member, rendeding smart pointers and assurance it gets deleted completely useless as the deletion is assured by the vector destructor and destructor of PNGmsgBase itself. note: it may be that returning a non pointer address to a base class with a derived class is not feasible through the usage of addresses, the only way would be by returning a const pointer to the element rather than the address
			message neutralMsg{noMsg()};	
			template<typename T>
			const message&  pngraise(T && e){
				msgLs.push_back(message(std::forward<T>(e)));
				if(PNGTRW) throw(e);
				else{
					if(!(quietMsg)) printMsg(msgLs.back(), softMsg, shortMsg);
					return msgLs.back();
				}
			}
			template<typename T>
			void pnglog(T && e){
				msgLs.push_back(message(std::forward<T>(e)));
				if(!(quietMsg)) printMsg(msgLs.back(), softMsg, shortMsg);
			}
			char * rcp(unsigned char * t) const {
				return reinterpret_cast<char *>(t);
			}
			unsigned char * pcr(char * t) const{
				return reinterpret_cast<unsigned char *>(t);
			}
			bool softMsg{true}, quietMsg{false}, shortMsg{true}, isClosed{false};
			
			uint64_t base_crc{crc32(0L, NULL, 0)};

			constexpr uint32_t btoi(const char * l) const noexcept{
				return 	(l[0] << 24) | (l[1] << 16) | (l[2] << 8) | l[3];
			}	
			constexpr uint32_t btoi(const unsigned char * l) const noexcept{
				return 	(l[0] << 24) | (l[1] << 16) | (l[2] << 8) | l[3];
			}	
			uint32_t btoi(std::string l) const noexcept{
				return btoi(pcr(l.data()));
			}
			const char * itob(const uint32_t l)  noexcept{
				for(int i{3}; i>=0; i--) buf2[3-i] = static_cast<char>((l >> (8*i) & 255));
				return buf2.data();
			}	
			constexpr void printHex(const char * l,size_t s) const noexcept{
				for(size_t i{}; i<s; i++) std::printf("%#x ", l[i]);
				std::printf("\n");
			}
			constexpr void printHex(const unsigned char * l, size_t s) const noexcept{
				for(size_t i{}; i<s; i++) std::printf("%#x ", l[i]);
				std::printf("\n");
			}
			void dumpHex(const char *fn, std::vector<unsigned char> & s) const noexcept{
				std::ofstream file_out{fn, std::ios::out};
				file_out.write(rcp(s.data()), s.size());
				file_out.close();
			}

			std::string buf1{"\0\0\0\0", 4};
			std::string buf2{"\0\0\0\0", 4};
			z_stream buffStream;
	};
}

