#include "myvector.h"
#include <iostream>
using namespace std;

float MyVector::dotproduct(int dim,float id1[], float id2[])
{
    float result = 0;
	for (int i = 0; i < dim; i++)
	{
		result += id1[i]*id2[i];
	}
	return result;
}

//compute the l2 square distance of two points
float MyVector::distancel2sq(int dim,float id1[], float id2[], float bound)// compute the distance between id1 and id2
{
	// to be optimized: use bound to filt
	float result = 0;
	for (int i = 0; i < dim; i++)
	{
		result += (id1[i] - id2[i])*(id1[i] - id2[i]);
	}
	if(result < 0) cout<<"error negative distance" <<endl;
	return result;
}
