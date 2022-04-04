#pragma once

//second library guard is unnecessary but..
#ifndef __PNGLIB__
#define __PNGLIB__

#include <iostream>
#include <cstdint>
#include <exception>
#include <zlib.h>
#include <string_view>
#include <cstring>
#include "PNGerr.h"

class PNG{
	public:
		constexpr PNG() noexcept : fn{}, bPNGios{nullptr}{};
		constexpr PNG(const std::string_view filename) noexcept : fn{filename}, bPNGios{nullptr}{/* open(filename)*/};
		//constexpr PNG(const PNG& a) noexcept{
		//open(std::string_view)
		constexpr const std::string_view & fileName() const noexcept{return fn;};

		//declared for consistency and -weffc++ warnings suppression 
		constexpr PNG& operator=(const PNG& t) = default;
		constexpr PNG(const PNG&) = default;
		virtual ~PNG() noexcept {delete [] bPNGios;};
		
		std::ios_base::iostate fsstate() noexcept{
			return bPNGios->rdstate(); //WILL cause segmentation fault if bPNGios is NOT initialized to point to a fstream object - this is expected in implementation of std::basic_ios, should there be an exception throw?
		}	
		
		//
		bool basic_check(); //checks for health of the file: position of chunks, whether the length is correct, missing critical chunks
		bool full_check(); //basic_check() + CRC check + [IDAT data segment decompression test]
		bool repair(); //runs basic_check() and full_check(), attempts to repair  chunks by calculating the correct CRC or chunk length
		bool force_repair(); //will force repairing of the segments by repositioning them following a specific order as described in the ancillary enum

		void test(){
			for(size_t i{}; i<=fn.size(); i++){
			//	std::printf("%d\n", static_cast<int>(fn[i]));
			}
			std::cout << (anChunks[3].data) << "\n";
			uint8_t cmptest[]{0xAE, 0x42, 0x60, 0x82};
			std::printf("%d\n", strncmp((const char *)IEND.crc.data(), (const char *) cmptest, 4));
			IDAT.size = 4;
		}	
			
	protected:
		
		
		//types declarations string_view and chunk_t
		//using sv=std::string_view; //
		class chunk_t {
			public:
			constexpr chunk_t() noexcept: size{}, pos{}, name{}, data{nullptr}, crc{}{};
			//constexpr chunkStruct_t(uint32_t s, std::initializer_list<uint8_t> n, uint8_t * d, std::initializer_list<uint8_t>  c) : size{s}, data{d}{std::copy(n.begin(), n.end(), name); std::copy(c.begin(), c.end(), crc);}; failed std::initializer_list ctor
			constexpr chunk_t(uint32_t s, uint32_t p, std::string_view n, uint8_t * d, std::string_view c) : size{s}, pos{p}, name{n}, data{d}, crc{c}{};
		
			//Useless??? If = delete Won't compile unless defined(???). Cannot delete these functions because it says they are being used in the PNG class constructor for some ungodly reason. THey have no reason to exist and they are not being used anywhere since the number of chunks is well defined and constant
			constexpr chunk_t(const chunk_t & t) noexcept = default;
			constexpr chunk_t& operator=(const chunk_t & t) = default;
			~chunk_t() noexcept{ delete [] data;};
			
			
			uint32_t size; 
			uint32_t pos; //position in the file of the beginning of the chunk, starting from the 4 bytes long chunk size sequence. e.g. pos is ALWAYS 8 (sigSize) for IHDR and it's NEVER 0 for ANY chunk - will be used for file health checking operations
			std::string_view name; //std::string_view better for optimizations and faster to deal with, especially for safe constructor of the chunk_t and destruction compared to static array of chars/uint8_t
			uint8_t * data; 
			std::string_view crc;

		};
		
		
		
		//enums of internal chunk constants, ancillary chinks order indexes, IHDR bits array indexes
		enum constants : int8_t	{infoSize=4, ihdrBitsNum=5, sigSize=8, anChunksNum=14}; 
		enum critical : int8_t 	{ihdr, plte, idat, iend};
		enum ancillary : int8_t {tRNS, cHRM, gAMA, iCCP, sBIT, sRGB, iTXt, tEXt, zTXt, bKGD, hIST, pHYs, sPLT, tIME};
		enum ihdrbits : int8_t	{depth, color, compression, filter, interlace};

		//std::string_view for critical and ancillary chunks identification names
		static constexpr std::string_view critID[infoSize]{"IHDR", "PLTE", "IDAT", "IEND"};
		static constexpr std::string_view ancID[anChunksNum]{"tRNS", "cHRM", "gAMA", "iCCP", "sBIT", "sRGB", "iTXt", "zTXt", "bKGD", "hIST", "pHYs", "sPLT", "tIME"};
		
		//width, height, [bit depth, color scheme, compression method, filter type, interlacing method], these are all included in the IHDR chunk
		uint32_t width{}, height{};
		int8_t ihdrBits[5]{};

		
		
		//Critical Chunks chunk_t variables
		static constexpr uint8_t signature[]{137,80,78,71,13,10,26,10}; //this is technically not a chunk
		//static constexpr chunk_t IHDR{13, {49, 48, 44, 52}, nullptr, {}}; same as IEND
		chunk_t IHDR{13,8, critID[ihdr], nullptr, {}};
		chunk_t PLTE{0,0,critID[plte], nullptr, {}}; //though it's not necessary if the palette color bit 
		chunk_t IDAT{0,0,critID[idat], nullptr, {}};
		//static constexpr chunk_t IEND{0, {0x49, 0x45, 0x4E, 0x44}, nullptr, {0xAE, 0x42, 0x60, 0x82}}; std::initializer_list needed for custom struct ctor- how?
		//static constexpr chunk_t IEND{0, critID[iend] , nullptr, "\xAE\x42\x60\x82"}; failed static constexpr declaration of IEND with chunk_t type  - weird g++ error
		const chunk_t IEND{0, 0,  critID[iend] , nullptr, "\xAE\x42\x60\x82"}; //chunk_t for conformity	
		//chunk_t array of ancillary chunks
		chunk_t anChunks[14]{};
		
		
		
		//file interface variables
		const std::string_view fn;
		std::basic_ios<char> *bPNGios;
		constexpr PNG(std::string_view f, std::basic_ios<char> *pbs) noexcept : fn{f}, bPNGios{pbs}{}; //protected constructor for the derived classes
		//bool wasInitialized; //could be implemented in the future to throw an exception when doing ANYTHING in the base class that's dependent on the fstream when the fstream has NOT been implemented yet - might be ok to check bPNGios as boolean?
};



#endif
