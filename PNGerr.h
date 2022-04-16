#pragma once
#include "PNGerrwarnbase.h"

namespace PNG{
	namespace PNGerr{
		struct PNGbasicErr : public PNGmsgBase{
			const char * type() const noexcept override{
				return "\033[1;31mError\033[0m";
			}
			virtual uint16_t code() const noexcept = 0;
			operator bool() const noexcept override{
				return true;
			}
		};
		struct testErr : public PNGbasicErr{
			testErr() noexcept {val = "This is an Error test Message.";};
			uint16_t code() const noexcept { return 0;};
		};

		namespace io_err{
			struct io_file : public PNGbasicErr{
				io_file(const char* l) noexcept {val = std::string("Fatal I/O Error in File with filename \"")+ l + "\", it either does not exist or it is inaccessible.";};
				uint16_t code() const noexcept override{
					return 1;
				};
			};

			
			struct io_stream : public PNGbasicErr{
				io_stream(const char* l) noexcept { val = std::string("Fatal I/O Error with the file stream in file with filename \"") + l + "\", cannot read or write to the file.";
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
				io_close(const char* l) noexcept{ val = std::string("Error Closing  \"") + l +"\" . Failbit has been set while attempting to close the file. Either the file has already been closed, it has not yet been opened or there is a problem with the file I/O.";};
				uint16_t code() const noexcept override{return 4;};
			};
			struct file_end_prematurely : public PNGbasicErr{
				file_end_prematurely(const char *l) noexcept{val = std::string("File with filename \"")+l+"\" ended prematurely before the IEND chunk.";};
				uint16_t code() const noexcept{return 11;};
			};

		}
		namespace chunk_err{
			struct no_signature : public PNGbasicErr{
				no_signature(const char * l) noexcept {val = std::string("No PNG signature has been detected on file \"") + l + "\".";};
				uint16_t code() const noexcept{return 5;};
			};
			struct bad_chunk_size : public PNGbasicErr{
				bad_chunk_size(const char * l) noexcept { val  = std::string("Bad ")+l+" chunk size, different than 13.";};
				uint16_t code() const noexcept { return 6;};
			};
			struct bad_chunk_name : public PNGbasicErr{
				bad_chunk_name(const char * l) noexcept { val = std::string("Bad chunk name for chunk \"") + l + "\".";};
				uint16_t code() const noexcept { return 7;};
			};
			struct bad_IHDR_bits : public PNGbasicErr{
				bad_IHDR_bits(const char *l) noexcept{ val = std::string("Bad IHDR information bits for the image: ") + l;};
				uint16_t code() const noexcept { return 8;};
			};	
			struct unsupported : public PNGbasicErr{
				unsupported(const char *l) noexcept{val = std::string(l) + " not supported.";};
				uint16_t code() const noexcept{ return 9;};
			};
			struct bad_crc32 : public PNGbasicErr{
				bad_crc32(const char *l) noexcept{val = std::string("Invalid crc32 redundancy check for chunk ")+l+".";};
				uint16_t code() const noexcept{return 10;};
			};
			struct no_plte_bit : public PNGbasicErr{
				no_plte_bit(const char *l) noexcept{ val = std::string("PLTE chunk found when the colour type bit is not 3 in file \"")+l+"\".";};
				uint16_t code() const noexcept{return 11;};
			};
			struct no_plte_found : public PNGbasicErr{
				no_plte_found(const char *l) noexcept{ val = std::string("PLTE color bit is set, but no PLTE chunk was found before IDAT in file \"")+l+"\".";};
				uint16_t code() const noexcept{return 12;};
			};
			struct multiple_plte : public PNGbasicErr{
				multiple_plte(const char *l) noexcept{ val = std::string("Multiple PLTE chunks were found in file \"")+l+"\".";};
				uint16_t code() const noexcept{return 14;};
			};
			struct unrecognized : public PNGbasicErr{
				unrecognized(const char *l) noexcept{val = std::string("Unrecognized chunk with name \"")+l+"\" found.";};
				uint16_t code() const noexcept{return 13;};
			};
		}

	}
}

