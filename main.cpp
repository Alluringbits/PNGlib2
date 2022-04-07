
#include <cstdio>
#include "iPNGlib.h"
#include <iostream>
#include <fstream>
#include <exception>

int main(){
	PNG::PNGTRW = false;
	PNG::iPNG test2{};
	try{
		PNG::printMsg(test2.open("main"));

	}
	catch(std::exception &e){
		std::cout << e.what() << "\n";
	}
	return 0;
}
