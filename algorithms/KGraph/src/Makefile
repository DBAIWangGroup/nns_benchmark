CC=g++ 

#ARCH=-msse2
#OPENMP=-fopenmp

OPT=-O3

boost_dir="/home/yingz/Software/boost_1_58_0/"
boost_lib="/usr/local/lib"

CXXFLAGS+=-fPIC -g -std=c++11 -I${boost_dir} -I. $(OPT) -L${boost_lib}  $(OPT) $(ARCH) $(OPENMP)
LDFLAGS+=-static $(OPENMP)
LDLIBS+=-lboost_timer -lboost_chrono -lboost_system -lboost_program_options -lgomp -lm -lrt  -L${boost_lib}

COMMON=kgraph.o metric.o RandGen.o

HEADERS=kgraph.h kgraph-data.h RandGen.h

PROGS=kgraph_index kgraph_search 

RELEASE_SRC=Makefile LICENSE kgraph.h kgraph-data.h kgraph_index.cpp kgraph_search.cpp 

$(PROGS): %:	%.cpp $(HEADERS) $(COMMON)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $*.cpp $(COMMON) $(LDLIBS)
%.o:	%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $*.cpp 

clean:

	rm -f *.o

