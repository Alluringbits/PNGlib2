#pragma once

#include "PNGlib.h"
#include <fstream>

class iPNG : public PNG{
	public:
		iPNG() noexcept : PNG("", &PNGifs), PNGifs{}{};
		iPNG(std::string_view f) :  PNG(f, &PNGifs), PNGifs(f.data(), std::ios::in){};
		//read PNGlib.h for explanation
		iPNG(const iPNG &t) = delete;
		iPNG & operator=(const iPNG &t) = delete;
		iPNG(iPNG && t) noexcept = default;
		iPNG& operator=(iPNG && t) noexcept = default;

		std::exception* open(std::string_view t) PNGEXC override;

		void test2(){
		};

		~iPNG() noexcept override {bPNGios = nullptr;};
	private:
		//conversion function for 4 bytes of chars to single integer	
		
		//file stream interface variables
		std::ifstream PNGifs;
};
