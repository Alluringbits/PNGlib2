#pragma once

#include "PNGlib.h"

namespace PNG{
	class oPNG: public basic_PNG{
		public:

		private:
			//bool copy(const basic_PNG& t); //replacement for the copy ctors, ONLY copies DATA (IDAT, PLTE, IHDR, width, height and other non-constexpr non-static variables but NOT the filename), can't be applied well if the PNG derived classes haven't been initialised (e.g. without filename and without initialization of the std::*fstream obj)
	};
}
