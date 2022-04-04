#pragma once

#include "PNGlib.h"
#include <fstream>

class iPNG : public PNG{
	public:
		iPNG() noexcept : PNG(PNG::fn.data(), &PNGifs), PNGifs{}{};
		iPNG(std::string_view f) noexcept :  PNG(f, &PNGifs), PNGifs(f.data(), std::ios::in){};
		//read PNGlib.h for explanation
		iPNG(const iPNG &t) = delete;
		iPNG & operator=(const iPNG &t) = delete;
		iPNG(iPNG && t) noexcept = default;
		iPNG& operator=(iPNG && t) noexcept = default;

		void test2(){
			//std::cout << "in test2\n";
			std::string x;
			PNGifs >> x;
			std::cout << x << "\n";
			uint8_t *tsta{new uint8_t[13]{67, 67}};
			IHDR.data = tsta;
			std::cout << IHDR.data[0] << "\n";
		}
		~iPNG() noexcept {};
	private:
		//conversion function for 4 bytes of chars to single integer	

		//file stream interface variables
		std::ifstream PNGifs;
};
