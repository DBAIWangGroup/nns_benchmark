#ifndef KNN_H_INCLUDED
#define KNN_H_INCLUDED

#include "constants.h"
#include "myvector.h"
#include "utility"
#include "algorithm"
using namespace std;
class Knn{
    public:
    int knnlist[K];
    float distlist[K];
    float sqrtbound;
    private:
    int tochange;
    float bound;

    private:
    struct dist_pair{
        float first;
        int second;
        dist_pair(float a,int b){first=a; second=b;}
    };

    public:
    void init();
    void linear_scan(float [][D], float []);
    void addvertex(float [][D], int, float []);
    void KNNsort();

    private:
    void computebound();
};

#endif // KNN_H_INCLUDED
