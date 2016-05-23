##############
# Make the SRS project
##############

PROD	:= DEBUG
OPT     := -O3 -march=native #-ffast-math 
VERSION := \"0.1_${PROD}\"
TARGETS := NSH
DEFINES := #-DREAL_PROF
SRCS    := NSH.cpp

CCFLAGS = ${OPT} -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -I./ -DVERSION=${VERSION}
LDFLAGS = ${OPT} -ggdb  
LIBS    = 
CC	= g++ 
OBJS    := ${SRCS:.cpp=.o}


.PHONY: all clean distclean 
all:: ${TARGETS} 

srs: NSH.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $< 

clean:: 
	-rm -f *~ *.o ${TARGETS}

distclean:: clean
