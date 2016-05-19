CC=g++ 
ARCH=-msse2
OPT=-O3
OPENMP=-fopenmp
VERSION=$(shell git describe --always)
BUILD_INFO=-DKGRAPH_VERSION=\"$(VERSION)\" -DKGRAPH_BUILD_ID=\"$(BUILD_ID)\" -DKGRAPH_BUILD_NUMBER=\"$(BUILD_NUMBER)\" #-DPARALLEL -DDISTANCE_SP
CXXFLAGS+=$(BUILD_INFO) -fPIC -Wall -g -std=c++11 -I/home/yzhang4/HD/home/liwen/Downloads/software/boost_1_58_0/ -I. $(OPT) -L/home/yzhang4/HD/home/liwen/Downloads/software/boost1.58/lib #$(OPENMP) $(OPT) $(ARCH)
LDFLAGS+=-static $(OPENMP)
LDLIBS+=-lboost_timer -lboost_chrono -lboost_system -lboost_program_options -lgomp -lm -lrt -L/home/yzhang4/HD/home/liwen/Downloads/software/boost1.58/lib 

# FLANN_LIBS+=-lflann_cpp_s -lflann_s
#NABO_LIBS+=-lnabo

#.PHONY:	all python clean release deps-ubuntu flann

COMMON=kgraph.o metric.o RandGen.o
HEADERS=kgraph.h kgraph-data.h RandGen.h
PROGS=main 


#deps-ubuntu:
#	apt-get install -y libboost-timer-dev libboost-chrono-dev libboost-program-options-dev libboost-system-dev libboost-python-dev libflann-dev python-numpy

#RELEASE=kgraph-release
RELEASE_SRC=Makefile LICENSE kgraph.h kgraph-data.h main.cpp
# RELEASE_BIN=libkgraph.so $(PROGS) #$(FLANN_PROGS)

$(PROGS): %:	%.cpp $(HEADERS) $(COMMON)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $*.cpp $(COMMON) $(LDLIBS)
%.o:	%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $*.cpp 

clean:
	rm -f $(PROGS) *.o

