#include <cstdio>
#include "iPNGlib.h"
#include "PNGerrwarnbase.h"
#include <iostream>
#include <fstream>
#include <exception>

int main(int argc, char **argv){
	/*z_stream tstSr;
	inflateInit(&tstSr);*/
	PNG::PNGTRW = false;
	PNG::iPNG test2{argv[1], "vs"};
	PNG::message a{PNG::PNGerr::testErr()};
	std::printf("%d\t%s\t%s\n", a.code(), a.type().data(), a.what().data());
	std::cout << bool(a) << "\n";
	test2.load();
	test2.close();
	return 0;
}
