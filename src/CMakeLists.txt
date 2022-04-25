add_subdirectory(PNGmessages)


add_library(iPNGlib iPNGlib.cpp)
add_library(oPNGlib oPNGlib.cpp)
add_library(PNGlib PNGlib.cpp)
target_precompile_headers(iPNGlib PUBLIC iPNGlib.h)
target_precompile_headers(oPNGlib PUBLIC oPNGlib.h)
target_precompile_headers(PNGlib PUBLIC PNGlib.h)
target_precompile_headers(PNGlib PUBLIC PNGincludes.h)

