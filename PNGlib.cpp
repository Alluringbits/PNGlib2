#include "PNGlib.h"

namespace PNG{
	void basic_PNG::reset() noexcept{
		width = 0;
		height = 0;
		IHDR.reset();
		IHDR.size = ihdrSize;
		IHDR.pos = sigSize;
		IHDR.exist = true;
		PLTE.reset();
		std::for_each(IDAT.begin(), IDAT.end(), [](chunk_t m){m.reset();});
		std::for_each(IDAT.begin(), IDAT.end(), [](chunk_t m){m.exist = true;});
		for(size_t i{}; i<anChunksNum; i++) anChunks[i].reset();
	}

}
