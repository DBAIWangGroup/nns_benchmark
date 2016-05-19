CC=g++ 

#ARCH=-msse2
OPT=-O3
#OPENMP=-fopenmp
boost_dir="/home/yingz/Software/boost_1_58_0/"
boost_lib="/usr/local/lib"

CXXFLAGS+=-fPIC -g -std=c++11 -I${boost_dir} -I. $(OPT) -L${boost_lib}  $(OPT) $(ARCH) 
LDFLAGS+=-static $(OPENMP)
LDLIBS+=-lboost_timer -lboost_chrono -lboost_system -lboost_program_options -lgomp -lm -lrt  -L${boost_lib}

COMMON=kgraph.o metric.o RandGen.o

HEADERS=kgraph.h kgraph-data.h RandGen.h

PROGS=DPG_index DPG_diverse DPG_search

RELEASE_SRC=Makefile LICENSE kgraph.h kgraph-data.h DPG_index.cpp DPG_search.cpp DPG_diverse.cpp

$(PROGS): %:	%.cpp $(HEADERS) $(COMMON)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $*.cpp $(COMMON) $(LDLIBS)
%.o:	%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $*.cpp 

clean:
	rm -f *.o




