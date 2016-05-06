#ifndef SHSELECTION_H_INCLUDED
#define SHSELECTION_H_INCLUDED

class SHSelection;
#include "constants.h"
#include "data.h"
#include "cmath"
#include <string>

using namespace std;

class SHSelection{
    public:

    private:
    int decisionsignal[datasize];
    int hashkeylength[L][bucketnum];

    public:
    //SHSelection();
    void radius_selection(string decision_file);
    private:
    //void radius_test(int);
	int radius_test(int);
};

#endif // SHSELECTION_H_INCLUDED
