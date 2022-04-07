#pragma once
#include "PNGerrwarnbase.h"

namespace PNG{
	namespace PNGerr{
		struct PNGerrBase : public PNGmsg{
			const char * type() const noexcept override{
				return "Error";
			}
			virtual uint16_t code() const noexcept = 0;
		};
		namespace io_err{
			struct io_file : public PNGerrBase{
				io_file(const char* l) noexcept {val = std::string("Error with file i/o: File with filename \"") + l + "\" either does not exist or it is inaccessible.";};
				const char* type() const noexcept override{
					return "Error";
				};
				uint16_t code() const noexcept override{
					return 1;
				};
			};

			
			struct io_stream : public PNGerrBase{
				io_stream(const char* l) noexcept { val = std::string("Error with file i/o: Fatal Error with the file stream in file with filename \"") + l + "\", cannot read or write to the file.";
				}
				uint16_t code() const noexcept override{
					return 2;
				};
			};
		}

	}
}
//Implement file reopening message. Not error or warning?
		/*	struct already_opened : public PNGmsg{
				already_opened(const char* l)
				const char* what() const noexcept override{
					return "Error with file: file has already been opened.";
				};
			};*/

