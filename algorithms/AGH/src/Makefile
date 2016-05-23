##############
# Make the AGH project
##############

PROD	:= DEBUG
OPT     := -O3 -march=native 
VERSION := \"0.1_${PROD}\"
TARGETS := AGH
SRCS    := AGH.cpp

CCFLAGS = ${OPT} -Wno-deprecated -ggdb -D${PROD} -I./ -DVERSION=${VERSION}
LDFLAGS = ${OPT} -ggdb  
LIBS    = 
CC	= g++ 
OBJS    := ${SRCS:.cpp=.o}


.PHONY: all clean distclean 
all:: ${TARGETS} 

srs: AGH.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $< 

clean:: 
	-rm -f *~ *.o ${TARGETS}

distclean:: clean
