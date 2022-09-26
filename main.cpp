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
	auto gammaChnk{test2.ancillaryM(PNG::text)};
	auto timeChunk{test2.ancillaryS(PNG::time)};
	std::printf("%d\t%s\n", gammaChnk.front().size, gammaChnk.front().name.data());
	PNG::printHex(timeChunk.data);
	for(auto a : gammaChnk){ for(auto i{gammaChnk.front().data.cbegin()}; i< gammaChnk.front().data.cend(); i++) (*i) ? std::printf("%c",*i) : std::printf(" "); std::printf("\n");}


	test2.close();
	return 0;
}
