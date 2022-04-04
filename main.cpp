#include <cstdio>
#include "PNGlib.h"
#include <iostream>
#include <fstream>
#include <exception>

int main(){
	PNG test{"test"};	
	//std::printf("%s\n", test.fileName().data());
	test.test();
	std::string t;
	std::ifstream file_in{"main.cpp", std::ios::in};
	//std::basic_ios<char> *ftestbase{new std::ifstream{"main.cpp", std::ios::in}};
	try{
		//file_in.open("main.cp", std::ios::in);
		file_in >> t;
		std::cout << file_in.fail() << "\n";
		std::bad_alloc err;
		//throw(err);
	}
	catch(std::exception &e){
		std::cout << e.what() << "\n";
	}
	file_in.close();
	return 0;
}
