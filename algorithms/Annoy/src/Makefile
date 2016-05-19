CC=g++
CCFLAGS=-O3 -lm -fPIC -fomit-frame-pointer -fno-stack-protector 
CPP_FILES=annoy.cpp
OBJ_FILES=$(CPP_FILES:%.cpp=%.o)

# Compile a single file.
%.o: %.cpp %.h
	$(CC) $(CCFLAGS) -c $<

# Main target.
all: indexer searcher
	rm -f *.o

indexer: $(OBJ_FILES) indexer.o
	rm -f $@
	g++ -fPIC $(OBJ_FILES) indexer.o -o $@

searcher: $(OBJ_FILES) searcher.o
	rm -f $@
	g++ -fPIC $(OBJ_FILES) searcher.o -o $@

clean: 
	rm -f *~ *.o
	rm indexer searcher

# Declare phony targets.
.PHONY: all clean

