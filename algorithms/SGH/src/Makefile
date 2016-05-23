##############
# Make the SGH project
##############

PROD	:= DEBUG
OPT     := -O3 -march=native #-ffast-math 
VERSION := \"0.1_${PROD}\"
TARGETS := SGH
DEFINES := #-DREAL_PROF
SRCS    := SGH.cpp

CCFLAGS = ${OPT} -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -I./ -DVERSION=${VERSION}
LDFLAGS = ${OPT} -ggdb  
LIBS    = 
CC	= g++ 
OBJS    := ${SRCS:.cpp=.o}


.PHONY: all clean distclean 
all:: ${TARGETS} 

srs: SGH.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $< 

clean:: 
	-rm -f *~ *.o ${TARGETS}

distclean:: clean
