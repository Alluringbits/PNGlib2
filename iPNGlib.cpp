#include "iPNGlib.h"

extern bool PNGTRW;

namespace PNG{
	const std::unique_ptr<PNGmsg>& iPNG::open(std::string_view t) PNGEXC {
			
		fn = t;
		PNGifs.open(fn.data(), std::ios::in);
		if(PNGifs.fail()){
			return pngraise(PNGerr::io_err::io_file(fn.data()));
		}
		return neutralMsg;
	}
}

