
#include <cstdio>
#include "iPNGlib.h"
#include <iostream>
#include <fstream>
#include <exception>

int main(){
	PNG::PNGTRW = false;
	PNG::iPNG test2{"manul.png", "vs"};
	test2.load();
	test2.close();
	return 0;
}
