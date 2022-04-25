
add_library(PNGerr PNGerr.cpp)
target_precompile_headers(PNGerr PUBLIC PNGerr.h)

#target_precompile_headers(a.out PUBLIC PNGmsg.h)
#target_precompile_headers(a.out PUBLIC PNGerrwarnbase.h)	
#target_precompile_headers(a.out PUBLIC PNGwarn.h)
