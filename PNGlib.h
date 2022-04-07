#pragma once

//second library guard is unnecessary but..
#ifndef ___PNGLIB__
#define ___PNGLIB__

#include "PNGerr.h"
#include <iostream>
#include <cstdint>
#include <exception>
#include <zlib.h>
#include <string_view>
#include <cstring>
#include <vector>
#include <initializer_list>
#include "PNGwarn.h"
#include "PNGerrwarnbase.h"

#ifndef __PNGEXC
	#undef PNGEXC 
	#define PNGEXC 
#else
	#define PNGEXC 
#endif

extern bool PNGTRW;


/*#define RAISE(a) if(PNGTRW){throw(a);} \
	else{delete lastErr;lastErr = new a; return *lastErr;}*/

#define PNGRAISE(a) if(PNGTRW){throw(a);}\
	else{errLs.push_back(new a); return errLs.back();}
#define PNGWARN(a) errLs.push_back(new a);

class PNG{
	public:
		PNG() noexcept : fn{}, bPNGios{nullptr}{};
		PNG(const std::string_view filename) noexcept : fn{filename}, bPNGios{nullptr}{/* open(filename)*/};
		virtual std::exception* open(std::string_view) PNGEXC = 0;
		constexpr const std::string_view  fileName() const noexcept{return fn;};

		//std::basic_istream and std::basic_ostream have the copy ctors deleted, since the file interface is done through that, only the move ctors will be declared. A separated function can copy the data contents from one object PNG to another object PNG
		PNG& operator=(const PNG& t) = delete;
		PNG(const PNG& t) = delete;
		//move ctors for consistency and usefulness
		PNG(PNG && t) noexcept = default;
		PNG& operator=(PNG && t) noexcept = default;
		virtual ~PNG() noexcept {bPNGios = nullptr;};//FOr safety sets to nullptr the std::basic_ios<char> ptr
		
		std::ios_base::iostate fsstate() noexcept{
			return bPNGios->rdstate(); //WILL cause segmentation fault if bPNGios is NOT initialized to point to a fstream object - this is expected in implementation of std::basic_ios, should there be an exception throw?
		}	

		const PNGewb * lastErr() noexcept{
			return errLs[0];
		}
		const std::vector<PNGewb *> errList() noexcept{
			return errLs;
		}


		bool copy(const PNG& t); //replacement for the copy ctors, ONLY copies DATA (IDAT, PLTE, IHDR, width, height and other non-constexpr non-static variables but NOT the filename), can't be applied well if the PNG derived classes haven't been initialised (e.g. without filename and without initialization of the std::*fstream obj)
		//non constexpr, non noexcept, they are supposed to throw custom PNG std::exception s defined in PNGerr.h
		bool basic_check(); //checks for health of the file: position of chunks, whether the length is correct, missing critical chunks
		bool full_check(); //basic_check() + CRC check + [IDAT data segment decompression test]
		bool repair(); //runs basic_check() and full_check(), attempts to repair  chunks by calculating the correct CRC or chunk length
		bool force_repair(); //will force repairing of the segments by repositioning them following a specific order as described in the ancillary enum

		void test(){
			for(size_t i{}; i<=fn.size(); i++){
			//	std::printf("%d\n", static_cast<int>(fn[i]));
			}
			//std::cout << (anChunks[3].data) << "\n";
			uint8_t cmptest[]{0xAE, 0x42, 0x60, 0x82};
			std::printf("%d\n", strncmp((const char *)IEND.crc.data(), (const char *) cmptest, 4));
			IDAT.size = 4;
		}	
			
	protected:
		
		
		//types declarations string_view and chunk_t
		//using sv=std::string_view; //
		class chunk_t {//usage of chunk_t::data requires first to resize it to the chunk_t::size value, then the chunk_t::data::data() can be used to return the pointer for the read(*,n) func
			public:
			chunk_t() noexcept: size{}, pos{}, name{}, data{}, crc{}{};
			//constexpr chunkStruct_t(uint32_t s, std::initializer_list<uint8_t> n, uint8_t * d, std::initializer_list<uint8_t>  c) : size{s}, data{d}{std::copy(n.begin(), n.end(), name); std::copy(c.begin(), c.end(), crc);}; //failed std::initializer_list ctor
			chunk_t(uint32_t s, uint32_t p, std::string_view n, std::initializer_list<uint8_t> d, std::string_view c) : size{s}, pos{p}, name{n}, data{d}, crc{c}{data.shrink_to_fit();};
		
			chunk_t(const chunk_t & t) noexcept = default;
			
			//deprecated, usage of C-array for data
			//constexpr chunk_t(const chunk_t & t) noexcept : size{t.size}, pos{t.pos}, name{t.name}, data{static_cast<uint8_t *>(std::calloc(size, sizeof(uint8_t)))}, crc{t.crc}{std::strncpy(static_cast<char *>(data), static_cast<char *>(t.data), size);}; //this one works equivalently, no assurance on memory security with the 
			chunk_t& operator=(const chunk_t & t) noexcept = default;
			
			//move ctors, for consistency
			chunk_t(chunk_t && t) noexcept = default;
			chunk_t& operator=(chunk_t && t) noexcept = default;
			~chunk_t() noexcept{};
			
			
			uint32_t size; 
			uint32_t pos; //position in the file of the beginning of the chunk, starting from the 4 bytes long chunk size sequence. e.g. pos is ALWAYS 8 (sigSize) for IHDR and it's NEVER 0 for ANY chunk - will be used for file health checking operations
			std::string_view name; //std::string_view better for optimizations and faster to deal with, especially for safe constructor of the chunk_t and destruction compared to static array of chars/uint8_t
			
			//deprecated, usage of C-array, std::array preferred
			//uint8_t * data; //arrays must always be dynamically allocated. THe destructor uses the operator delete, every data array for the chunk data must be and has to be dynamically allocated. debate whether to use std::string_view or basic uint8_t* array
			
			std::vector<uint8_t> data; //due to  memory allocation issues it may be better to use std::vector rather than raw pointers
			std::string_view crc;

		};
		
		
		//enums of internal chunk constants, ancillary chinks order indexes, IHDR bits array indexes
		enum constants : int8_t	{infoSize=4, ihdrBitsNum=5, sigSize=8, anChunksNum=14}; 
		enum critical : int8_t 	{ihdr, plte, idat, iend};
		enum ancillary : int8_t {tRNS, cHRM, gAMA, iCCP, sBIT, sRGB, iTXt, tEXt, zTXt, bKGD, hIST, pHYs, sPLT, tIME};
		enum ihdrbits : int8_t	{depth=8, color, compression, filter, interlace};

		//std::string_view for critical and ancillary chunks identification names
		static constexpr std::string_view critID[infoSize]{"IHDR", "PLTE", "IDAT", "IEND"};
		static constexpr std::string_view ancID[anChunksNum]{"tRNS", "cHRM", "gAMA", "iCCP", "sBIT", "sRGB", "iTXt", "zTXt", "bKGD", "hIST", "pHYs", "sPLT", "tIME"};
		
		//width, height, [bit depth, color scheme, compression method, filter type, interlacing method], these are all included in the IHDR chunk
		uint32_t width{}, height{};
		
	

		//Critical Chunks chunk_t variables
		static constexpr uint8_t signature[]{137,80,78,71,13,10,26,10}; //this is technically not a chunk
		//static constexpr chunk_t IHDR{13, {49, 48, 44, 52}, nullptr, {}}; same as IEND
		chunk_t IHDR{13,8, critID[ihdr], {}, {}};
		chunk_t PLTE{0,0,critID[plte], {}, {}}; //though it's not necessary if the palette color bit is not set, the PNG standard classifies it as critical - conformity
		chunk_t IDAT{0,0,critID[idat], {}, {}};
		//static constexpr chunk_t IEND{0, {0x49, 0x45, 0x4E, 0x44}, nullptr, {0xAE, 0x42, 0x60, 0x82}}; std::initializer_list needed for custom struct ctor- how?
		//static constexpr chunk_t IEND{0, critID[iend] , nullptr, "\xAE\x42\x60\x82"}; failed static constexpr declaration of IEND with chunk_t type  - weird g++ error
		const chunk_t IEND{0, 0,  critID[iend] , {}, "\xAE\x42\x60\x82"}; //chunk_t for conformity	
		//chunk_t array of ancillary chunks
		chunk_t anChunks[14]{};
		
		
		
		//file interface variables
		std::string_view fn;
		std::basic_ios<char> *bPNGios;
		PNG(std::string_view f, std::basic_ios<char> *pbs) noexcept : fn{f}, bPNGios{pbs}{}; //protected constructor for the derived classes
		//bool wasInitialized; //could be implemented in the future to throw an exception when doing ANYTHING in the base class that's dependent on the fstream when the fstream has NOT been implemented yet - might be ok to check bPNGios as boolean?
		
		//last error storage variable
		//PNGewb *lastErr{new PNGwarn::noWarn()};

		//error vector list - after some consideration there will be no handling of errors in a std::queue system with popping of first error after it has been requested by the client - might be worth looking into it in the future for a possible efficient and functional impl
		std::vector<PNGewb *> errLs{};
};


#endif
