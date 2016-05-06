#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include "constants.h"
#include "io.h"
#include "knn.h"
#include "SHindex.h"
#include "SHgeneral.h"
#include "SHselection.h"
#include "statisticsmodule.h"

extern float data[datasize][D];
extern IO io;
extern Knn knn;
extern SHGeneral shg;
extern SHSelection shs;
extern SHIndex shi;
extern StatisticsModule st;

#endif //DATA_H_INCLUDED
