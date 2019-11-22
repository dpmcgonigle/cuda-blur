#   Variables
#   Linking libraries needed
#       lboost_program_options for command-line options
#       lpthread for CImg, for whatever reason
CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lboost_program_options -lpthread
SOURCES=main.cpp utils.cpp cimg_utils.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=blur.exe

#   Linking; No output
all: $(SOURCES) $(EXECUTABLE)

#   Compiling Executable
#   Example output: g++ main.o utils.o -o blur.exe -lboost_program_options
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

#   Compiling Sources
#   Build .o from .cpp, Special variables $@ and $< expand to the target and first dependency respectively
#   Example output: g++ main.cpp -o main.o -c -Wall; g++ utils.cpp -o utils.o -c -Wall
.cpp.o:
	$(CC) $< -o $@ $(CFLAGS)






#   ORIGINAL, SIMPLER MAKEFILE
##   Link command
#blur.exe: main.cpp utils.o
#	g++ -o blur.exe main.cpp utils.o -lboost_program_options
#
##   Compilation commands
#utils.o: utils.cpp
#	g++ -c utils.cpp -o utils.o
