CXXFLAGS =  -Wall -pedantic -pedantic-errors -g -ansi -Wextra -Weffc++ -std=c++17 -O3
O=./ObjectFiles
FILES = $(O)/main.o $(O)/PNGlib.o $(O)/oPNGlib.o $(O)/iPNGlib.o
EXECNAME=a.out
all: dir $(EXECNAME)
.PHONY: all clean 

dir:
	mkdir -p $(O) 
clean:
	rm $(O)/*.o
esegui:
	./$(EXECNAME)

$(EXECNAME): $(FILES)
	g++ $(CXXFLAGS) $^ $(LIBS) -o $@

$(O)/%.o: %.cpp 
	g++ $(CXXFLAGS) -c $< $(LIBS) -o $@

