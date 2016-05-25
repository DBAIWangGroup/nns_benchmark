#ifndef MYRANDOM_H_INCLUDED
#define MYRANDOM_H_INCLUDED

#include <stdlib.h>
class MyRandom{
    public:
    static int int_random(int);
    static void rand_multi_gaussian(float [], int);
    static float rand_single_gaussian();
};

#endif // MYRANDOM_H_INCLUDED
