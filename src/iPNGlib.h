#pragma once

#include "PNGlib.h"
#include <fstream>

namespace PNG{
	class iPNG : public basic_PNG{
		public:
			iPNG() noexcept : basic_PNG("", &PNGifs), PNGifs{}{};
			iPNG(std::string_view f, std::string_view flags = "ss") :  basic_PNG(f, &PNGifs, flags), PNGifs{}{(*this).open(f);};
			//read PNGlib.h for explanation
			iPNG(const iPNG &t) = delete;
			iPNG & operator=(const iPNG &t) = delete;
			iPNG(iPNG && t) noexcept = default;
			iPNG& operator=(iPNG && t) noexcept = default;

			const message & open(std::string_view t) PNGEXC override;
			const message & close() override;

			const message & load();
			const message & repair(); //runs load() if not loaded already attempts to repair  chunks by calculating the correct CRC or chunk length
			void test2(){
			};

			~iPNG() noexcept override {if(!isClosed) (*this).close();bPNGios = nullptr;};
		private:
			//conversion function for 4 bytes of chars to single integer	
			//file stream interface variables
			std::ifstream PNGifs;
			
			const message& readChunk();
			const message& readIHDR();
			const message& readPLTE();
			const message& readIDAT();
			const message& readAncS(int i);
			const message& readAncM(int i);
			const message& decompr();
			const message& defilter();

			std::streamoff fs{};
			bool isLoaded{false};
	};
}
