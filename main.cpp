
#include <cstdio>
#include "iPNGlib.h"
#include <iostream>
#include <fstream>
#include <exception>


int main(){
	PNGTRW = false;
	iPNG test2{};
	try{
		std::cout << test2.open("noexists")->what() << "\n";
		std::cout << test2.lastErr()->what() << "\n";
		std::cout << test2.lastErr()->code() << "\n";
	}
	catch(std::exception &e){
		std::printf("test\n");
		std::cout << e.what() << "\n";
	}
	return 0;
}
