#pragma once
#include "PNGerrwarnbase.h"

namespace PNG{
	namespace PNGerr{
		struct PNGbasicErr : public PNGmsgBase{
			const std::string_view type() const noexcept override{
				return tp;
			}
			virtual uint16_t code() const noexcept = 0;
			operator bool() const noexcept override{
				return true;
			}
			private:
				std::string_view tp{"\033[1;31mError\033[0m"};
		};
		struct testErr : public PNGbasicErr{
			testErr() noexcept {val = "This is an Error test Message.";};
			uint16_t code() const noexcept { return 92233;};
		};

		namespace io_err{
			struct io_file : public PNGbasicErr{
				io_file(const char* l) noexcept {val = std::string(l)+": Fatal I/O Error. The file either does not exist or it is inaccessible.";};
				uint16_t code() const noexcept override{
					return 1;
				};
			};

			
			struct io_stream : public PNGbasicErr{
				io_stream(const char* l) noexcept { val = std::string(l)+": Fatal I/O Error with the file stream, cannot read or write to the file.";
				}
				uint16_t code() const noexcept override{
					return 2;
				};
			};
			struct open_new : public PNGbasicErr{
				open_new(const char * n, const char * l) noexcept { val = std::string("Error Reopening file \"") + n + "\" with new file with filename \"" + l +"\".";};
				uint16_t code() const noexcept override{return 3;};
			};
			struct io_close : public PNGbasicErr{
				io_close(const char* l) noexcept{ val = std::string(l)+": Error Closing the File. Failbit has been set while attempting to close the file. Either the file has already been closed, it has not yet been opened or there is a problem with the file I/O.";};
				uint16_t code() const noexcept override{return 4;};
			};
			struct file_end_prematurely : public PNGbasicErr{
				file_end_prematurely(const char *l) noexcept{val = std::string(l)+": The File ended prematurely before the IEND chunk.";};
				uint16_t code() const noexcept override {return 15;};
			};
			struct empty_filename : public PNGbasicErr{
				empty_filename(const char *l) noexcept{val= std::string(l)+": An empty filename was provided.";};
				uint16_t code() const noexcept override {return 21;};
			};

		}
		namespace chunk_err{
			struct no_signature : public PNGbasicErr{
				no_signature(const char * l) noexcept {val = std::string(l)+": No PNG signature has been detected at the start of the File.";};
				uint16_t code() const noexcept override {return 5;};
			};
			struct bad_chunk_size : public PNGbasicErr{
				bad_chunk_size(const char *fn, const char * l) noexcept { val  = std::string(fn)+": Bad "+l+" chunk size.";};
				uint16_t code() const noexcept override { return 6;};
			};
			struct bad_chunk_name : public PNGbasicErr{
				bad_chunk_name(const char *fn, const char * l) noexcept { val = std::string(fn)+ ": Bad chunk name for chunk \"" + l + "\".";};
				uint16_t code() const noexcept override { return 7;};
			};
			struct bad_IHDR_bits : public PNGbasicErr{
				bad_IHDR_bits(const char *fn, const char *l) noexcept{ val = std::string(fn)+": Bad IHDR information bits, "+l+".";};
				uint16_t code() const noexcept override { return 8;};
			};	
			struct unsupported : public PNGbasicErr{
				unsupported(const char *fn, const char *l) noexcept{val = std::string(fn) + ": "+l+" not supported.";};
				uint16_t code() const noexcept override { return 9;};
			};
			struct bad_crc32 : public PNGbasicErr{
				bad_crc32(const char *fn, const char *l) noexcept{val = std::string(fn)+": Invalid crc32 redundancy check for chunk "+l+".";};
				uint16_t code() const noexcept override {return 10;};
			};
			struct no_plte_bit : public PNGbasicErr{
				no_plte_bit(const char *l) noexcept{ val = std::string(l)+": PLTE chunk found when the colour type bit is not 3.";};
				uint16_t code() const noexcept override {return 11;};
			};
			struct not_found : public PNGbasicErr{
				not_found(const char *fn, const char* l) noexcept{ val = std::string(fn)+": Critical chunk "+l+" not found when needed.";};
				uint16_t code() const noexcept override {return 12;};
			};
			struct multiple: public PNGbasicErr{
				multiple(const char *fn, const char* l) noexcept{ val = std::string(fn)+": Multiple declarations of the chunk "+l+" are not allowed.";};
				uint16_t code() const noexcept override {return 14;};
			};
			struct unrecognized : public PNGbasicErr{
				unrecognized(const char *fn, const char *l) noexcept{val = std::string(fn)+": Unrecognized chunk with name \""+l+"\" found.";};
				uint16_t code() const noexcept override {return 13;};
			};
			struct corrupted : public PNGbasicErr{
				corrupted(const char *l) noexcept{val=std::string(l)+": The compressed pixel data could not be decompressed as it is incomplete or corrupted.";};
				uint16_t code() const noexcept override {return 18;};
			};
			struct bad_filter_method  : public PNGbasicErr{
				bad_filter_method(const char *fn,int i) noexcept{ val = std::string(fn)+": Filter method with number "+std::to_string(i)+" is unrecognized.";};
				uint16_t code() const noexcept override {return 19;};
			};

		}
		
		struct version : public PNGbasicErr{
			version(const char *l) noexcept{ val = std::string(l)+": Incorrect zlib version. Unable to compress or decmpress.";};
			uint16_t code() const noexcept override { return 20;};
		};
		struct memory : public PNGbasicErr{
			memory() noexcept{ val = "Loading or creation of the file could not be completed due to insufficient system memory.";};
			uint16_t code() const noexcept{return 16;};
		};
		struct unexpected : public PNGbasicErr{
			unexpected(const char *l) noexcept{val = std::string(l)+": I'm sorry, Dave. I'm afraid I can't do that. Unexpected error.";};
			uint16_t code() const noexcept override {return 17;};
		};

	}
}

