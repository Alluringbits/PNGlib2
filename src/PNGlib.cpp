#include "PNGlib.h"

namespace PNG{
	void basic_PNG::reset() noexcept{
		width = 0;
		height = 0;
		IHDR.reset();
		IHDR.size = ihdrSize;
		IHDR.pos = sigSize;
		IHDR.exist = false;
		PLTE.reset();
		//std::for_each(IDAT.begin(), IDAT.end(), [](chunk_t m){m.reset(); m.exist = false;});
		IDAT.clear();
		for(size_t i{}; i<anChunksSNum; i++) anChunksS[i].reset();
		for(size_t i{}; i<anChunksMNum; i++) anChunksM[i].clear();
	}

		
	void basic_PNG::initanC(){
		for(size_t i{}; i<anChunksSNum; i++){
			anChunksS[i].reset();
			anChunksS[i].name = ancID[i];
		}
		for(size_t i{}; i<anChunksMNum; i++){
			anChunksM[i].clear();
		}
	}	

}
