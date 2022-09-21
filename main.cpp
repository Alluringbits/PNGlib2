#include <cstdio>
#include "iPNGlib.h"
#include "PNGerrwarnbase.h"
#include <iostream>
#include <fstream>
#include <exception>

int main(int argc, char **argv){
	/*z_stream tstSr;
	inflateInit(&tstSr);*/
	/* PNG::iPNG test2{argv[1], "vs"}; */
	PNG::iPNG test2{argv[1], PNG::Verbose | PNG::Short};
	test2.load();
	test2.close();
	return 0;
}
