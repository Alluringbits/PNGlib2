#pragma once
#include "PNGerrwarnbase.h"

namespace PNGerr{
	namespace io_err{
		struct already_opened : public PNGewb{
			const char* what() const noexcept override{
				return "Error with file: file has already been opened.";
			};
		};
		struct io_file : public PNGewb{
			io_file(std::string_view l) noexcept {val=l;};
			const char* what() const noexcept override{
				return "Error with file i/o: Either the file does not exist or it is inaccessible.";
			};
			uint16_t code() const noexcept override{
				return 1;
			}
		};
		struct io_stream : public PNGewb{
			const char* what() const noexcept override{
				return "Error with file i/o: Fatal error with the file stream, cannot read or write to the file.";
			};
		};
	};

}

