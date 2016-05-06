#include "myrandom.h"


int MyRandom::int_random(int max) // generate a random num from 0 to max-1 (max may be any unsigned int)
{
	long long int longresult;
	int i = 1;
	longresult = rand();
	while (longresult < 16*max)
	{
		i = 16 * i;
		longresult = 16*longresult + rand();
	}
	int result = longresult%max;
	return result;
}

//generate a simple multi gaussian distribution n dimensions variance matrix is I
void MyRandom::rand_multi_gaussian(float array[], int n)
{
	for (int i = 0; i < n; i++) array[i] = rand_single_gaussian();
}

// generate a random variable follows Gaussian distribution where mean = 0 varaince = 1
float MyRandom::rand_single_gaussian()
{
	int sum = 0;
	for (int i = 0; i < 256; i++)
	{
		sum += rand() % 2;
	}
	float gaussian  = (float)(sum-128)/8;
	// this part is to avoid repeat series
	int test = rand()%8;
	int sleep;
	for(int i = 0; i < test; i++) sleep = rand();
	return gaussian;
}
