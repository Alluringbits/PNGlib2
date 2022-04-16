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
		//pnglog(PNGmsg::file_opening(fn.data()));
		pnglog(PNGmessage(std::string("Opening File \"") + fn.data()+ "\"."));
		std::streamoff temppos{PNGifs.tellg()};
		PNGifs.seekg(0, std::ios_base::end);
		fs = PNGifs.tellg();
		PNGifs.clear();
		PNGifs.seekg(temppos, std::ios_base::beg);
		return neutralMsg;
	}
	const std::unique_ptr<PNGmsgBase>& iPNG::close(){
		PNGifs.close();
		isClosed = true;
		if(PNGifs.fail()) return pngraise(PNGerr::io_err::io_close(fn.data()));
		//pnglog(PNGmsg::file_closing(fn.data()));
		pnglog(PNGmessage(std::string("Closing File \"")+fn.data()+"\"."));
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::load(){
		try{
			PNGifs.exceptions(std::ifstream::failbit);
			if(*(readIHDR().get())) return msgLs.back();
			while(!IENDcomplete) if(*(readChunk().get())) return msgLs.back();
			return neutralMsg;
		}
		catch(const std::ios_base::failure & e){
			if(PNGifs.eof()){
				if(IENDcomplete){
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
		unsigned char tmpInfo[5];
		tmpInfo[4] = '\0';
		std::streamoff tempPos{PNGifs.tellg()};
		PNGifs.read(rcp(tmpInfo), infoSize);
		uint32_t chunkSize{btoi(tmpInfo)};
		if(chunkSize > fs) return pngraise(PNGerr::io_err::file_end_prematurely(fn.data()));
		PNGifs.read(rcp(tmpInfo), infoSize);
		if(!(std::strncmp(rcp(tmpInfo), critID[plte].data(),infoSize))){
			if(IHDR.data[color] == 3){
				if(PLTE.exist){
					return pngraise(PNGerr::chunk_err::multiple_plte(fn.data()));
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
		else if(!(std::strncmp(rcp(tmpInfo), critID[idat].data(), infoSize))){
			if((IHDR.data[color] == 3)){
				if(PLTE.exist){
					IDAT.push_back(chunk_t(chunkSize, tempPos, critID[idat], {}, {}, true));
					return readIDAT();
				}
				else{
					return pngraise(PNGerr::chunk_err::no_plte_found(fn.data()));
				}
			}
			else{
				IDAT.push_back(chunk_t(chunkSize, tempPos, critID[idat], {}, {}, true));
				return readIDAT();
			}
		}
		else if(!(std::strncmp(rcp(tmpInfo), critID[iend].data(), infoSize))){
			unsigned char crchunk[4];
			PNGifs.exceptions(std::ifstream::badbit);
			PNGifs.read(rcp(crchunk), infoSize);
			if(IEND.crc == crc32(base_crc, crchunk, 4)){
			       	IENDcomplete = true;
			}
			else{
				return pngraise(PNGerr::chunk_err::bad_crc32(critID[iend].data()));
			}
			PNGifs.clear();
			PNGifs.exceptions(std::ifstream::failbit);
			return neutralMsg;
		}
		else{
			for(size_t i{}; i<anChunksNum; i++){
				if(!(std::strncmp(rcp(tmpInfo), ancID[i].data(), infoSize))){
					anChunks[i].size = chunkSize;
					anChunks[i].pos = tempPos;
					return readAnc(i);
				}
			}
		}
		return pngraise(PNGerr::chunk_err::unrecognized(rcp(tmpInfo)));

		/*if(tmpInfo[0] == 'I'){
			PNGifs.get(tmpInfo[1]);
			if(tmpInfo[1] == 'E'){
				PNGifs.get(tmpInfo[2]);
				if(tmpInfo[2] == 'N'){
					PNGifs.get();
					PNGifs.exceptions(std::ifstream::badbit);
					PNGifs.read(tmpInfo, 4);
					if(IEND.crc == crc32(base_crc, reinterpret_cast<const unsigned char *>(tmpInfo), 4)) IENDcomplete = true;
					PNGifs.clear();
					std::cout << IENDcomplete << "\n";
					PNGifs.exceptions(std::ifstream::failbit);
				}
				
			}
		}*/
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readIHDR(){
			char readSig[sigSize];
			PNGifs.read(readSig, sigSize);
			if(std::strncmp(readSig, reinterpret_cast<const char * >(signature), sigSize)) return pngraise(PNGerr::chunk_err::no_signature(fn.data()));
			PNGifs.read(readSig, infoSize);
			if(btoi(readSig)!= ihdrSize) return pngraise(PNGerr::chunk_err::bad_chunk_size(critID[ihdr].data()));
			IHDR.data.resize(IHDR.size+infoSize);
			PNGifs.read(rcp(IHDR.data.data()), infoSize);
			if(std::strncmp(rcp(IHDR.data.data()), critID[ihdr].data(), infoSize)) return pngraise(PNGerr::chunk_err::bad_chunk_name(critID[ihdr].data()));
			PNGifs.read(rcp(IHDR.data.data()+infoSize), IHDR.size);
			width = btoi(IHDR.data.data()+infoSize);
			height = btoi(IHDR.data.data()+infoSize*2);
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
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits("Bad bit depth for Color type greyscale."));
							break;
					}
					break;
				case 2:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits("Bad bit depth for Color type Truecolor."));
							break;
					}
					break;
				case 3:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 1:
						case 2:
						case 4:
						case 8:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits("Bad bit depth for Color type Indexed Colour."));
							break;
					}
					break;
				case 4:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits("Bad bit depth for Color type Greyscale with Alpha."));
							break;
					}
					break;
				case 6:
					switch(static_cast<short unsigned int>(IHDR.data[depth])){
						case 8:
						case 16:
							break;
						default:
							return pngraise(PNGerr::chunk_err::bad_IHDR_bits("Bad bit depth for Color type Truecolour with Alpha."));
							break;
					}
					break;
				default:
					return pngraise(PNGerr::chunk_err::bad_IHDR_bits("Bad color type value."));
					break;
			}
			if(IHDR.data[compression]) return pngraise(PNGerr::chunk_err::unsupported("Compression methods different from DEFLATE/INFLATE are"));
			if(IHDR.data[filter]) return pngraise(PNGerr::chunk_err::unsupported("Line filtering methods are not implemented and are"));
			if(IHDR.data[interlace]) return pngraise(PNGerr::chunk_err::unsupported("Interlacing is"));
			IHDR.crc = crc32(base_crc, IHDR.data.data(), (IHDR.size+infoSize));
			PNGifs.read(readSig, infoSize);
			if(std::strncmp(itob(IHDR.crc), readSig, infoSize)) return pngraise(PNGerr::chunk_err::bad_crc32("IHDR"));
			return neutralMsg;
	}
	
	const std::unique_ptr<PNGmsgBase>& iPNG::readPLTE(){
		if(PLTE.exist) return pngraise(PNGerr::chunk_err::multiple_plte(fn.data()));
		PLTE.data.resize(PLTE.size);
		PNGifs.read(rcp(PLTE.data.data()), PLTE.size);
		unsigned char  crchunk[4];
		PNGifs.read(rcp(crchunk), infoSize);
		PLTE.exist = true;
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readIDAT(){
		IDAT.back().data.resize(IDAT.back().size);
		PNGifs.read(rcp(IDAT.back().data.data()), IDAT.back().size);
		unsigned char crchunk[4];
		PNGifs.read(rcp(crchunk), infoSize);
		return neutralMsg;
	}

	const std::unique_ptr<PNGmsgBase>& iPNG::readAnc(int i){
		//return pngraise(PNGerr::chunk_err::unsupported("Ancillary chunks are"));
		pnglog(PNGwarning(std::string("Ancillary chunk with name \"")+ancID[i].data()+"\" is not supported."));
		anChunks[i].data.resize(anChunks[i].size);
		PNGifs.read(rcp(anChunks[i].data.data()),anChunks[i].size);
		unsigned char crchunk[4];
		PNGifs.read(rcp(crchunk),infoSize);
		anChunks[i].exist = true;
		return neutralMsg;
	}
}

