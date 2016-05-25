#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include<string>

using namespace std;

class IO{
    public:
    void diskread_float(string filename, float array[], int size);
    void diskwrite_float(string filename, float array[], int size);
    void diskread_int(string filename, int array[], int size);
    void diskwrite_int(string filename, int array[], int size);
};

#endif // IO_H_INCLUDED
