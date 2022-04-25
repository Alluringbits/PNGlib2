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

			const std::unique_ptr<PNGmsgBase> & open(std::string_view t) PNGEXC override;
			const std::unique_ptr<PNGmsgBase> & close() override;

			const std::unique_ptr<PNGmsgBase> & load();
			const std::unique_ptr<PNGmsgBase> & repair(); //runs load() if not loaded already attempts to repair  chunks by calculating the correct CRC or chunk length
			void test2(){
			};

			~iPNG() noexcept override {if(!isClosed) (*this).close();bPNGios = nullptr;};
		private:
			//conversion function for 4 bytes of chars to single integer	
			//file stream interface variables
			std::ifstream PNGifs;
			
			const std::unique_ptr<PNGmsgBase>& readChunk();
			const std::unique_ptr<PNGmsgBase>& readIHDR();
			const std::unique_ptr<PNGmsgBase>& readPLTE();
			const std::unique_ptr<PNGmsgBase>& readIDAT();
			const std::unique_ptr<PNGmsgBase>& readAncS(int i);
			const std::unique_ptr<PNGmsgBase>& readAncM(int i);
			const std::unique_ptr<PNGmsgBase>& decompr();
			const std::unique_ptr<PNGmsgBase>& defilter();

			std::streamoff fs{};
			bool isLoaded{false};
	};
}
