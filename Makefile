#   Variables
#   Linking libraries needed
#       lboost_program_options for command-line options
#       lpthread for CImg, for whatever reason
#       lcudart for CUDA
CC=g++
CUDACC=nvcc
CFLAGS=-c -Wall
CUDACFLAGS=-c
LDFLAGS=-lboost_program_options -lpthread -lcudart
SOURCES=main.cpp utils.cpp cimg_utils.cpp
CUDASOURCES=cimg_utils_cuda.cu
OBJECTS=$(SOURCES:.cpp=.o)
CUDAOBJECTS=$(CUDASOURCES:.cu=.o)
EXECUTABLE=blur.exe

#   Linking; No output
all: $(SOURCES) $(CUDASOURCES) $(EXECUTABLE)
#all: $(SOURCES) $(EXECUTABLE)

#   Compiling Executable
#   Example output: g++ main.o utils.o -o blur.exe -lboost_program_options
$(EXECUTABLE): $(OBJECTS) $(CUDAOBJECTS)
	$(CC) $(OBJECTS) $(CUDAOBJECTS) -o $@ $(LDFLAGS)
#$(EXECUTABLE): $(OBJECTS)
#	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

#   Compiling Sources
#   Build .o from .cpp, Special variables $@ and $< expand to the target and first dependency respectively
#   Example output: g++ main.cpp -o main.o -c -Wall; g++ utils.cpp -o utils.o -c -Wall
.cpp.o:
	$(CC) $< -o $@ $(CFLAGS)

#   Compiling Cuda Sources
#.cu.o:
#	$(CUDACC) $(CUDACFLAGS) $(CUDASOURCES) -o $(CUDAOBJECTS)
cimg_utils_cuda.o: cimg_utils_cuda.cu
	nvcc -c cimg_utils_cuda.cu -o cimg_utils_cuda.o





#   ORIGINAL, SIMPLER MAKEFILE
##   Link command
#blur.exe: main.cpp utils.o
#	g++ -o blur.exe main.cpp utils.o -lboost_program_options
#
##   Compilation commands
#utils.o: utils.cpp
#	g++ -c utils.cpp -o utils.o
