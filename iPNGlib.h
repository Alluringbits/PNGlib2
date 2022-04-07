#pragma once

#include "PNGlib.h"
#include <fstream>

namespace PNG{
	class iPNG : public basic_PNG{
		public:
			iPNG() noexcept : basic_PNG("", &PNGifs), PNGifs{}{};
			iPNG(std::string_view f) :  basic_PNG(f, &PNGifs), PNGifs(f.data(), std::ios::in){};
			//read PNGlib.h for explanation
			iPNG(const iPNG &t) = delete;
			iPNG & operator=(const iPNG &t) = delete;
			iPNG(iPNG && t) noexcept = default;
			iPNG& operator=(iPNG && t) noexcept = default;

			const std::unique_ptr<PNGmsg> & open(std::string_view t) PNGEXC override;

			void test2(){
			};

			~iPNG() noexcept override {bPNGios = nullptr;};
		private:
			//conversion function for 4 bytes of chars to single integer	
			
			//file stream interface variables
			std::ifstream PNGifs;
	};
}
