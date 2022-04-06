#include "iPNGlib.h"

extern bool PNGTRW;


std::exception* iPNG::open(std::string_view t) PNGEXC {
		
	fn = t;
	PNGifs.open(fn.data(), std::ios::in);
	if(PNGifs.fail()){
		PNGRAISE(PNGerr::io_err::io_file(fn));
	}
	return (new PNGwarn::noWarn());
}

