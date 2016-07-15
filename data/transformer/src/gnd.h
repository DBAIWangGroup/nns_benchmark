#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <queue>  

using namespace std;
using std::pair;
using std::make_pair;

float get_dist(const float* p1, const float* p2, int d)
{
	float dist = 0;
	for(int i = 0; i < d ; i++ )
	{
		float tmp = p1[i]-p2[i];
		dist += tmp * tmp;
	}
	return dist;
}

struct _pair
{
	float dist;
	int id;
	friend bool operator<(_pair n1, _pair n2)
    {
         return n1.dist > n2.dist;
    }
};


int* get_gnd(const float* dataset, const float* query, int n, int nq, int d, int k)
{
	int* groundtruth = new int[nq*k];
	for(int i=0;i<nq;i++)
	{
		std::priority_queue<_pair> gnd;
		for(int j = 0; j < n; j++)
		{
			float dist = get_dist(&query[i*d], &dataset[j*d],d);
			_pair p;
			p.dist=dist;
			p.id=j;
			gnd.push(p);
		}
		for(int j=0;j<k;j++)
		{
			const _pair& top = gnd.top();
			groundtruth[i*k+j]=top.id;
			gnd.pop();
		}
	}
	return groundtruth;
}

