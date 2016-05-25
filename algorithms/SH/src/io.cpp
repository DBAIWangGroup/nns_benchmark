#include "io.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

void IO::diskread_float(string filename, float array[], int size)
{
    FILE *fp;
	fp = fopen(filename.c_str(),"rb");
	if(fp == NULL)
	{
	    cout << "Cannot open file!" << endl;
		exit(1);
	}
	fread(array, sizeof(double), size, fp);
	fclose(fp);
}

void IO::diskwrite_float(string filename, float array[], int size)
{
    FILE *fp;
	fp = fopen(filename.c_str(),"wb");
	if(fp == NULL)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
	fwrite(array, sizeof(float), size, fp);
	fclose(fp);
}

void IO::diskread_int(string filename, int array[], int size)
{
    FILE *fp;
	fp = fopen(filename.c_str(),"rb");
	if(fp == NULL)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
	fread(array, sizeof(int), size, fp);
	fclose(fp);
}

void IO::diskwrite_int(string filename, int array[], int size)
{
	FILE *fp;
	fp = fopen(filename.c_str(),"wb");
	if(fp == NULL)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
	fwrite(array, sizeof(int), size, fp);
	fclose(fp);
}
