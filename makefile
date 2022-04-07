CC = g++-10
CXXFLAGS =  -Wall -pedantic -pedantic-errors -g -ansi -Wextra -Weffc++ -std=c++20 -O3
O=./ObjectFiles
FILES = $(O)/main.o $(O)/PNGlib.o $(O)/oPNGlib.o $(O)/iPNGlib.o $(O)/PNGerr.o
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
	$(CC) $(CXXFLAGS) $^ $(LIBS) -o $@

$(O)/%.o: %.cpp 
	$(CC) $(CXXFLAGS) -c $< $(LIBS) -o $@

