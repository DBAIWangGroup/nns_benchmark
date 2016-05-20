##############
# Make the SRS project
##############

boost_dir="/home/yingz/Software/boost_1_58_0/"
boost_lib="/usr/local/lib"

PROD	:= DEBUG
OPT     := -O3 # -pg
VERSION := \"0.1_${PROD}\"
TARGETS := srs cal_param gen_gt gen_hard_data #name of binary file
DEFINES := #-DREAL_PROF
SRCS    := srs.cpp ProjData.cpp ParamFile.cpp RandGen.cpp SRSCoverTree.cpp cal_param.cpp gen_gt.cpp gen_hard_data.cpp

CCFLAGS = -std=c++11 ${OPT} -Wno-deprecated -ggdb -D${PROD} ${DEFINES} -I./ -DVERSION=${VERSION} -L${boost_lib} -I${boost_dir}
LDLIBS+= -L${boost_lib}

LDFLAGS = ${OPT} -ggdb  
LIBS    = 
CC	= g++ 
OBJS    := ${SRCS:.cpp=.o}


.PHONY: all clean distclean 
all:: ${TARGETS} 

srs: srs.o SRSCoverTree.o ParamFile.o ProjData.o RandGen.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

cal_para: cal_param.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

gen_gt: gen_gt.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

gen_hard_data: gen_hard_data.o RandGen.o
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.cpp
	${CC} ${CCFLAGS} -o $@ -c $< 

clean:: 
	-rm -f *~ *.o ${TARGETS}

distclean:: clean
