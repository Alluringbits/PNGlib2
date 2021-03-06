#include "iPNGlib.h"

extern bool PNGTRW;

namespace PNG{
	const std::unique_ptr<PNGmsgBase>& iPNG::open(std::string_view t) PNGEXC {
		if(PNGifs.is_open()) return pngraise(PNGerr::io_err::open_new(fn.data(), t.data()));
		fn = t;
		PNGifs.open(fn.data(), std::ios::binary);
		if(PNGifs.fail()) return pngraise(PNGerr::io_err::io_file(fn.data()));
		isClosed = false;
		//Quick test for whether the file can be read, if not it will set badbit
		PNGifs.get();
		PNGifs.unget();
		if(PNGifs.bad()) return pngraise(PNGerr::io_err::io_stream(fn.data()));
		pnglog(PNGmessage(std::string(fn.data())+": Opening the File."));
		std::streamoff temppos{PNGifs.tellg()};
		PNGifs.seekg(0, std::ios_base::end);
		fs = PNGifs.tellg();
		PNGifs.clear();
		PNGifs.seekg(temppos, std::ios_base::beg);
		return neutralMsg;
	}
	const std::unique_ptr<PNGmsgBase>& iPNG::close(){
		PNGifs.clear();
		try{
			isClosed = true;
			PNGifs.close();
		}
		catch(...){
			return pngraise(PNGerr::io_err::io_close(fn.data()));
		}
		pnglog(PNGmessage(std::string(fn.data())+": Closing the File."));
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::load(){
		buffStream.zalloc = Z_NULL;
		buffStream.zfree = Z_NULL;
		buffStream.opaque = Z_NULL;
		buffStream.avail_in = 0;
		buffStream.next_in = Z_NULL;
		switch(inflateInit(&buffStream)){
			case Z_MEM_ERROR:
				throw(PNGerr::memory());
				break;
			case Z_OK:
				break;
			case Z_VERSION_ERROR:
				return pngraise(PNGerr::version(fn.data()));
				break;
			case Z_STREAM_ERROR:
			default:
				return pngraise(PNGerr::unexpected(fn.data()));
				break;
		}
		try{
			PNGifs.exceptions(std::ifstream::failbit);
			if(*(readIHDR().get())) return msgLs.back();
			decompressedData.resize(static_cast<size_t>(1.1*(height+(width/bitsMult)*height*pixelBytes)));
			//rawPixelData.resize(decompressedData.size()-height);
			while(!(IEND.exist)) if(*(readChunk().get())) return msgLs.back();
			//if(*(decompr().get())) return msgLs.back(); //This was used when the inflation process was done in the end and not interleaved.
			if(*(defilter().get())) return msgLs.back();
			/*decompressedData.resize(buffStream.total_out); // for interleaved filtering- slower than final filtering
			rawPixelData.resize(buffStream.total_out-height);
			inflateEnd(&buffStream);*/
			return neutralMsg;
		}
		catch(const std::ios_base::failure & e){
			if(PNGifs.eof()){
				if(IEND.exist){
					PNGifs.clear();	
					return neutralMsg;
				}
				else{
					return pngraise(PNG::PNGerr::io_err::file_end_prematurely(fn.data()));
				}
			}
			else{
				return pngraise(PNG::PNGerr::io_err::io_stream(fn.data()));
			}
		}
		catch(const PNGmsgBase * e){
			throw(e);
		}
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readChunk(){
		std::streamoff tempPos{PNGifs.tellg()};
		PNGifs.read(buf1.data(), infoSize);
		uint32_t chunkSize{btoi(buf1)};
		PNGifs.read(buf1.data(), infoSize);
		if(!(buf1.compare(critID[plte]))){
			if(chunkSize > fs) return pngraise(PNGerr::chunk_err::bad_chunk_size(fn.data(), buf1.data()));
			if(IHDR.data[color] == 3){
				if(PLTE.exist){
					return pngraise(PNGerr::chunk_err::multiple(fn.data(), critID[plte].data()));
				}
				else{
					PLTE.size = chunkSize;
					PLTE.pos = tempPos;
					return readPLTE();
				}
			}
			else{
				return pngraise(PNGerr::chunk_err::no_plte_bit(fn.data()));
			}
		}
		else if(!(buf1.compare(critID[idat]))){
			if(chunkSize > fs) return pngraise(PNGerr::chunk_err::bad_chunk_size(fn.data(), buf1.data()));
			if((IHDR.data[color] == 3) && (!(PLTE.exist))) return pngraise(PNGerr::chunk_err::not_found(fn.data(), critID[plte].data()));
			else{
				IDAT.push_back(chunk_t(chunkSize, tempPos, critID[idat], {}, {}, true));
				return readIDAT();
			}
		}
		else if(!(buf1.compare(critID[iend]))){
			if(chunkSize > fs) return pngraise(PNGerr::chunk_err::bad_chunk_size(fn.data(), buf1.data()));
			if(!(IDAT.size())) return pngraise(PNGerr::chunk_err::not_found(fn.data(), critID[idat].data()));
			PNGifs.exceptions(std::ifstream::badbit);
			PNGifs.read(buf2.data(), infoSize);
			if(IEND.crc == crc32(base_crc, pcr(buf2.data()), 4)){
			       	IEND.exist = true;
			}
			else{
				return pngraise(PNGerr::chunk_err::bad_crc32(fn.data(),critID[iend].data()));
			}
			PNGifs.clear();
			PNGifs.exceptions(std::ifstream::failbit);
			return neutralMsg;
		}
		else{
			for(size_t i{}; i<anChunksSNum; i++){
				if(!(buf1.compare(ancID[i]))){
					if(chunkSize > fs) return pngraise(PNGerr::chunk_err::bad_chunk_size(fn.data(), buf1.data()));
					if(anChunksS[i].exist) return pngraise(PNGerr::chunk_err::multiple(fn.data(), ancID[i].data()));
					anChunksS[i].size = chunkSize;
					anChunksS[i].pos = tempPos;
					return readAncS(i);
				}
			}
			for(size_t i{}; i<anChunksMNum; i++){
				if(!(buf1.compare(ancID[i+anChunksSNum]))){
					if(chunkSize > fs) return pngraise(PNGerr::chunk_err::bad_chunk_size(fn.data(), buf1.data()));
					anChunksM[i].push_back(chunk_t(chunkSize, tempPos, ancID[anChunksSNum+i], {},  ancCRC[anChunksSNum+i], true));
					return readAncM(i);
				}
			}
		}
		if(!(buf1.compare(critID[ihdr]))) return pngraise(PNGerr::chunk_err::multiple(fn.data(), critID[ihdr].data()));
		return pngraise(PNGerr::chunk_err::unrecognized(fn.data(),buf1.data()));

	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readIHDR(){
			buf1.resize(sigSize);
			PNGifs.read(buf1.data(), sigSize);
			if(buf1.compare(0,sigSize, reinterpret_cast<const char *>(signature))) return pngraise(PNGerr::chunk_err::no_signature(fn.data()));
			buf1.resize(infoSize);
			PNGifs.read(buf1.data(), infoSize);
			if(btoi(buf1)!= ihdrSize) return pngraise(PNGerr::chunk_err::bad_chunk_size(fn.data(), critID[ihdr].data()));
			PNGifs.read(buf1.data(), infoSize);
			if(buf1.compare(critID[ihdr])) return pngraise(PNGerr::chunk_err::bad_chunk_name(fn.data(), critID[ihdr].data()));
			IHDR.data.resize(IHDR.size);
			PNGifs.read(rcp(IHDR.data.data()), IHDR.size);
			width = btoi(IHDR.data.data());
			height = btoi(IHDR.data.data()+infoSize);
			//big switch to check the correct matching of bit depth and Color type
			switch(static_cast<short unsigned int>(IHDR.data[color])){
				case 0:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 1:
						case 2:
						case 4:
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits(fn.data(),"Bad bit depth for Color type greyscale."));
							break;
					}
					pixelBytes = 1;
					break;
				case 2:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits(fn.data(),"Bad bit depth for Color type Truecolor."));
							break;
					}
					pixelBytes = 3;
					break;
				case 3:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 1:
						case 2:
						case 4:
						case 8:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits(fn.data(),"Bad bit depth for Color type Indexed Colour."));
							break;
					}
					pixelBytes = 1;
					break;
				case 4:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits(fn.data(),"Bad bit depth for Color type Greyscale with Alpha."));
							break;
					}
					pixelBytes = 2;
					break;
				case 6:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits(fn.data(),"Bad bit depth for Color type Truecolour with Alpha."));
							break;
					}
					pixelBytes = 4;
					break;
				default:
					return pngraise(PNGerr::chunk_err::bad_IHDR_bits(fn.data(),"Bad color type value."));
					break;
			}
			bitsMult = 8.f/static_cast<short unsigned int>(IHDR.data[depth]);
			if(IHDR.data[compression]) return pngraise(PNGerr::chunk_err::unsupported(fn.data(),"Compression methods different from DEFLATE/INFLATE are"));
			if(IHDR.data[filter]) return pngraise(PNGerr::chunk_err::unsupported(fn.data(),"Line filtering methods are not implemented and are"));
			if(IHDR.data[interlace]) return pngraise(PNGerr::chunk_err::unsupported(fn.data(),"Interlacing is"));
			IHDR.crc = crc32(critCRC[ihdr], IHDR.data.data(), IHDR.size);
			PNGifs.read(buf1.data(), infoSize);
			if(btoi(buf1) != IHDR.crc) return pngraise(PNGerr::chunk_err::bad_crc32(fn.data(),"IHDR"));
			IHDR.exist = true;
			return neutralMsg;
	}
	
	const std::unique_ptr<PNGmsgBase>& iPNG::readPLTE(){
		if(PLTE.exist) return pngraise(PNGerr::chunk_err::multiple(fn.data(), critID[plte].data()));
		PLTE.data.resize(PLTE.size);
		PNGifs.read(rcp(PLTE.data.data()), PLTE.size);
		PLTE.crc = crc32(critCRC[plte], PLTE.data.data(), PLTE.size);
		PNGifs.read(buf1.data(), infoSize);
		if(btoi(buf1) != PLTE.crc) return pngraise(PNGerr::chunk_err::bad_crc32(fn.data(),"PLTE"));
		PLTE.exist = true;
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readIDAT(){
		/*size_t startPos{compressedData.size()};
		compressedData.resize(startPos+IDAT.back().size);
		PNGifs.read(rcp(compressedData.data()+startPos), IDAT.back().size);
		IDAT.back().crc = crc32(critCRC[idat], compressedData.data()+startPos, IDAT.back().size);*/

		IDAT.back().data.resize(IDAT.back().size);
		PNGifs.read(rcp(IDAT.back().data.data()), IDAT.back().size);
		IDAT.back().crc = crc32(critCRC[idat], IDAT.back().data.data(), IDAT.back().size);
		
		PNGifs.read(buf1.data(), infoSize);
		if(btoi(buf1) != IDAT.back().crc) return pngraise(PNGerr::chunk_err::bad_crc32(fn.data(), "IDAT"));
		return decompr();
		//return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readAncS(int i){
		pnglog(PNGwarning(std::string("Ancillary chunk with name \"")+ancID[i].data()+"\" is not supported."));
		anChunksS[i].data.resize(anChunksS[i].size);
		PNGifs.read(rcp(anChunksS[i].data.data()),anChunksS[i].size);
		anChunksS[i].crc = crc32(ancCRC[i], anChunksS[i].data.data(),  anChunksS[i].size);
		PNGifs.read(buf1.data(),infoSize);
		if(btoi(buf1) != anChunksS[i].crc) return pngraise(PNGerr::chunk_err::bad_crc32(fn.data(),ancID[i].data()));
		anChunksS[i].exist = true;
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readAncM(int i){
		pnglog(PNGwarning(std::string("Ancillary chunk with name \"")+ancID[i+anChunksSNum].data()+"\" is not supported."));
		anChunksM[i].back().data.resize(anChunksM[i].back().size);
		PNGifs.read(rcp(anChunksM[i].back().data.data()), anChunksM[i].back().size);
		anChunksM[i].back().crc = crc32(ancCRC[i+anChunksSNum], anChunksM[i].back().data.data(), anChunksM[i].back().size);
		PNGifs.read(buf1.data(), infoSize);
		if(btoi(buf1) != anChunksM[i].back().crc) return pngraise(PNGerr::chunk_err::bad_crc32(fn.data(), ancID[i+anChunksSNum].data()));
		anChunksM[i].back().exist = true;
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::decompr(){
		/*size_t totalBitSize{static_cast<size_t>(1.1*(height+width*height*pixelBytes))};
		decompressedData.resize(totalBitSize);
		switch(uncompress(decompressedData.data(), &totalBitSize ,compressedData.data(), compressedData.size())){*/
		buffStream.next_in = IDAT.back().data.data();
		buffStream.avail_in = IDAT.back().data.size();
		buffStream.next_out = decompressedData.data()+buffStream.total_out;
		buffStream.avail_out = decompressedData.size()-buffStream.total_out;
		switch(inflate(&buffStream, Z_NO_FLUSH)){
			case Z_OK:
			case Z_STREAM_END:
				break;
			case Z_MEM_ERROR:
				//look after carefully, insufficient memory in nowadays hardware is extremely improbable and in case there is insufficient memory, std::string wouldn't be able to allocate (making PNGmsgBase useless because it needs std::string allocation. it's therefore made to throw.
				throw(PNGerr::memory());
				break;
			case Z_DATA_ERROR:
				return pngraise(PNGerr::chunk_err::corrupted(fn.data()));
				break;
			default:
				return pngraise(PNGerr::unexpected(fn.data()));
				break;
		}
		//decompressedData.resize(totalBitSize);
		return neutralMsg;
		//return defilter(); //for interleaved filtering
	}
	const std::unique_ptr<PNGmsgBase>& iPNG::defilter(){
		size_t rawPxlRow{static_cast<size_t>(width/bitsMult)*pixelBytes};
		decompressedData.resize(buffStream.total_out);
		rawPixelData.resize(decompressedData.size()-height);
		inflateEnd(&buffStream);
		/*static size_t j{};
		size_t rowsSoFar{static_cast<size_t>(buffStream.total_out/(rawPxlRow+1))};
		//std::cout << j << "\t" << rowsSoFar << "\n";
		for(;j<rowsSoFar; j++){*/
		for(size_t j{}; j<height; j++){
			size_t tmpRow{j*rawPxlRow};
			size_t tmpRowA{tmpRow-pixelBytes};
			size_t tmpRowB{tmpRow-rawPxlRow};
			size_t tmpRowC{tmpRowA-rawPxlRow};
			size_t tmpRowCompr{tmpRow+j+1};
			//std::printf("Filtering method: %d\n", decompressedData[tmpRow+j]);
			switch(decompressedData[tmpRowCompr-1]){
				case 0:
					for(size_t i{}; i<rawPxlRow; i++){
						rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i];
					}
					break;
				case 1:
					std::memcpy(rawPixelData.data(), decompressedData.data()+j+1, pixelBytes);
					for(size_t i{pixelBytes}; i<rawPxlRow; i++){
						rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i]+rawPixelData[tmpRowA+i];
					}
					break;
				case 2:
					if(!j){
					       	std::memcpy(rawPixelData.data(), decompressedData.data()+1, rawPxlRow);
					}
					else{
						for(size_t i{}; i<rawPxlRow; i++){
							rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i]+rawPixelData[tmpRowB+i];
						}
					}
					break;
				case 3:
					if(!j){
						std::memcpy(rawPixelData.data(), decompressedData.data()+j+1, pixelBytes);
						for(size_t i{pixelBytes}; i<rawPxlRow; i++){
							rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i]+std::floor(rawPixelData[tmpRowA+i]/2);
						}
					}
					else{
						for(size_t i{}; i<pixelBytes; i++){
							rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i]+std::floor(rawPixelData[tmpRowB+i]/2);
						}
						for(size_t i{pixelBytes}; i<rawPxlRow; i++){
							rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i]+std::floor((static_cast<uint16_t>(rawPixelData[tmpRowA+i])+static_cast<uint16_t>(rawPixelData[tmpRowB+i]))/2);
						}

					}
					break;
				case 4:
					//std::printf("Paeth filtering method used in row number %ld.\n", j);
					for(size_t i{}; i<rawPxlRow; i++){
						int32_t a{(i<pixelBytes) ? 0 : rawPixelData[tmpRowA+i]};
						int32_t b{(!j) ? 0 : rawPixelData[tmpRowB+i]};
						int32_t c{(!j) ? 0 : ((i<pixelBytes ) ? 0 : rawPixelData[tmpRowC+i])};
						int32_t p{a+b-c}, pa{std::abs(p-a)}, pb{std::abs(p-b)}, pc{std::abs(p-c)};
						int32_t pr{(pa<= pb && pa <= pc) ? a : ((pb<=pc) ? b : c)};
						rawPixelData[tmpRow+i] = decompressedData[tmpRowCompr+i] +pr;

					}
					break;
				default:
					//printHex(decompressedData.data()+tmpRowCompr-5, 10);
					return pngraise(PNGerr::chunk_err::bad_filter_method(fn.data(), decompressedData[tmpRowCompr-1]));
					break;
			}
			
		}
		//printHex(rawPixelData.data(), rawPixelData.size());
		return neutralMsg;
	}
}

