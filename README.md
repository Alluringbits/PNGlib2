# PNGlib2
Custom  PNG library in C++

## Purpose
This is a custom PNG library made entirely for fun and exercise. It's not meant (as of now) to be fully functional, though it technically can be used.
By no means it is a replacement for official and better PNG libraries. Its aim is to be an efficient, useful, exception-based  clas-based C++ library for PNG images.

The way it is structured is by having a single base PNG class and two derived oPNG (output) and iPNG (input) classes to write or read PNG images. It will be provided with image checking and repairing methods.


## Compiling
To compile this library it's only necessary to use the basic instructions for Cmake. Being the first usage of cmake the building process is basilar and just generates the final executable in the same build folder. In order to build and compile it a building folder that cmake will use has to be created wherever wanted:
```
mkdir build
cd build
```
Then the command `cmake` will have to be used in it, giving it the path to the library root folder where the  CMakeLists.txt is present:
```
cmake <path>
```
And so it can be built:
```
cmake --build .
```
This will create in the folder the executable `a.out` which is just used for testing of the library through the `main.cpp` file. For now, installing instructions in the cmake file are not supported.
