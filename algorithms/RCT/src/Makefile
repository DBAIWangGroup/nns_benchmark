# Project setup.
CC=g++
CCFLAGS=-O3 -lm -fPIC -fomit-frame-pointer -fno-stack-protector # '-fno-stack-protector' used to fix an issue with 'ld' in GCC 4.X+!
CPP_FILES=random.cpp rct.cpp DistData.cpp
OBJ_FILES=$(CPP_FILES:%.cpp=%.o)
H_FILES=$(CPP_FILES:%.cpp=%.h)

# Compile a single file.
%.o: %.cpp %.h
	$(CC) $(CCFLAGS) -c $<

# Main target.
all: rct.a rct.so indexer searcher doc
	rm -f *.o

indexer: $(OBJ_FILES) indexer.o
	rm -f $@
	g++ -fPIC $(OBJ_FILES) indexer.o -o $@

searcher: $(OBJ_FILES) searcher.o
	rm -f $@
	g++ -fPIC $(OBJ_FILES) searcher.o -o $@

# Build static library.
rct.a: $(OBJ_FILES)
	rm -f $@
	ar cq $@ $(OBJ_FILES)

# Build shared library.
rct.so: $(OBJ_FILES)
	rm -f $@
	#ld -G $(OBJ_FILES) -o $@
	g++ -shared $(OBJ_FILES) -o $@ 
# Clean up.
clean: 
	rm -f *~ *.o rct.a rct.so
	rm -rf html latex dist
	rm indexer searcher

# Declare phony targets.
.PHONY: all clean doc

# Documentation
#doc: rct.h rct.cpp
	#doxygen rct.h rct.cpp
