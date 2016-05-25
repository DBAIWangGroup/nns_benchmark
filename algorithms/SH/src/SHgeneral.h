#ifndef SHGENERAL_H_INCLUDED
#define SHGENERAL_H_INCLUDED

class SHGeneral;

#include "constants.h"
#include "data.h"
#include "myvector.h"
#include "myrandom.h"
#include <string>

using namespace std;

class SHGeneral{
    private:
    
    float dataproduct[datasize][familysize];
    float familyvector[familysize][D+1];
    int hashtableindex[L][M];
    unsigned int datahashresult[datasize][L];
    int datahashtable[L][datasize];
    int decision[datasize];
    bool decisionavailable;
    bool isinit;

    public:
	float R[Alter];
    friend class SHSelection;
    friend class SHIndex;

    public:
    SHGeneral();
    void init();
    void tableindex(float [], int, unsigned int []);

    private:
    void family_generator();
    void generate_hashtableindex();
    void familysample(int result[], int size, int needsize);
    void productcomputer();
};

#endif // SHGENERAL_H_INCLUDED
