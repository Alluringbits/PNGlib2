cmake_minimum_required(VERSION 3.19)

find_package(ZLIB)

project(PNGlib VERSION 1.0)

#SET(GCC_FLAGS "-Wall -g -pedantic -pedantic-errors -ansi -Wextra -Weffc++ -std=c++20 -O3")
SET(GCC_FLAGS "-Wall -g -pedantic -pedantic-errors -ansi -Wextra -Weffc++ -std=c++20 -O0")
add_definitions(${GCC_FLAGS})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_executable(a.out main.cpp)

target_link_libraries(a.out PUBLIC iPNGlib oPNGlib PNGlib PNGerr z)

target_include_directories(a.out PUBLIC
       	"${PROJECT_BINARY_DIR}"
       	"${PROJECT_SOURCE_DIR}/src"
       	"${PROJECT_SOURCE_DIR}/src/PNGmessages"
	)
