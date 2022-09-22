#include <cstdio>
#include "iPNGlib.h"
#include "PNGerrwarnbase.h"
#include "auxiliary.h"
#include <iostream>
#include <fstream>
#include <exception>

int main(int argc, char **argv){
	/*z_stream tstSr;
	inflateInit(&tstSr);*/
	/* PNG::iPNG test2{argv[1], "vs"}; */
	PNG::iPNG test2{argv[1], PNG::Verbose | PNG::Short};
	test2.load();
	/* const_cast<std::array<PNG::chunk_t, 10>&>(test2.ancillarySingle())[PNG::gama].size = 1; */
	auto gammaChnk{std::get<PNG::chunk_t>(test2.ancillary(PNG::gama))};
	std::printf("%d\t%s\n", gammaChnk.size, gammaChnk.name.data());
	PNG::printHex(gammaChnk.data);

	test2.close();
	return 0;
}
