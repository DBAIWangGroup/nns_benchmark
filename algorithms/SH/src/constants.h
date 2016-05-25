#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED


const int D = 192;
const int datasize = 53387;


const int K = 20;
const int L = 80;
const int M = 30;
const int querysize = 200;
const int familysize = 100;
//selective hashing specific
const int Alter = 20;
const float BaseR = 100;
const float c = 1.2;
const int bucketnum = 9973;
const int thresholdpoint = (int)(3*datasize/bucketnum);
const float thresholdtable = (int)(0.4*L);
const float ETRatio = 0.3;

/* 
// This is for ERON data
const int D = 1369;
const int K = 20;

const int L = 80;
const int M = 30;
const int datasize = 94987;
const int querysize = 200;
const int familysize = 100;
//selective hashing specific
const int Alter = 20;
const float BaseR = 100;
const float c = 3;
const int bucketnum = 9973;
const int thresholdpoint = (int)(3*datasize/bucketnum);
const float thresholdtable = (int)(0.4*L);
const float ETRatio = 0.3;
//for basic LSH test
//const int Alter = 1;
//const double BaseR = 5;
//const int Checked[16]={100,200,400,500,700,900,1000,1500,2000,3000,5000,10000,15000,20000,30000};
*/

#endif // CONSTANTS_H_INCLUDED
