#include <cstring>
#include <malloc.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <getopt.h>
#include <cstdlib>
#include <dirent.h>
#include <stdexcept>
#include "DistData.h"
//#include "VecData.h"

#ifndef _DATA_UTIL_H
#define _DATA_UTIL_H

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;

DistData** ReadPoints(const char * filepath, int *len, int *d) {
	FILE * dfp = fopen(filepath, "rb");
	int dim;
	fread(&dim,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(float)*dim));
	*d = dim;
	*len = n;
	DistData ** points = new DistData*[n];
	for(int i=0 ; i<n ; i++)
  	{
		float * data = new float [dim];
		fread(&dim, sizeof(int), 1, dfp);
		fread(data, sizeof(float), dim, dfp);
		DistData* a=new DistData(data,dim);
		points[i]= a;
		delete data;
  	}
	fclose(dfp);
	return points;
}

void ReadPoints(const char * filepath,DistData** points, int *len, int *d) {
	FILE * dfp = fopen(filepath, "rb");
	int dim;
	fread(&dim,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(float)*dim));
	*d = dim;
	*len = n;
	points = new DistData*[n];
	for(int i=0 ; i<n ; i++)
  	{
		float * data = new float [dim];
		fread(&dim, sizeof(int), 1, dfp);
		fread(data, sizeof(float), dim, dfp);
		DistData* a=new DistData(data,dim);
		points[i]= a;
		delete data;
  	}
	fclose(dfp);
}

void ReadGroundtruth(const string& filename,int** gnds, int count) {
  ifstream input;
	input.open(filename.c_str(), ios::binary);
  if(!input.good()) {
    throw std::logic_error("Invalid filename");
  }
  int nn;
  for(int pid = 0; pid < count; ++pid) {
    input.read((char*)&nn, sizeof(nn));
    if(nn <= 0) {
      throw std::logic_error("Bad file content: non-positive dimension");
    }
	int* gnd=(int*)malloc(sizeof(int)*nn);//new float[dimension];
    for(int d = 0; d < nn; ++d) {
      int buffer;
      input.read((char*)&(buffer), sizeof(int));
      gnd[d] = buffer;	  
    }    
	gnds[pid]= gnd;
  }
}

void ReadChecks(const string& filename, vector<float>* checks) {
  FILE *fp = fopen(filename.c_str(), "r");
  int n;
  fscanf(fp, "%f ", &n);
  checks->resize(n);
  int tmp;
  for (int i = 0; i < n; ++i) {
    fscanf(fp, "%f ", &tmp);
    checks->at(i)=tmp;
  }
}

float compute_recall(int** gnds, int** indices,int nq,int nn,int* nums)
{
	int avg = 0;
	for (size_t i=0; i<nq; ++i) {
		for (size_t j=0;j<nums[i];++j) {
			for (size_t k=0;k<nn;++k) {
				if (indices[i][j]==gnds[i][k]) {
					avg ++;
				}
			}
		}
	}
	return float(avg)/(nn*nq);
}

float compute_mean_reciprocal_rank(int** gnds, int** indices,int nq,int nn,int* nums)
{
	float avg = 0;
	for (size_t i=0; i<nq; ++i) 
	{
		float rate=0.0;
		int j=0;
		//for(int j=0;j<nn;j++)
		{
			for(int k=0;k<nums[i];k++)
			{
				if(gnds[i][j]==indices[i][k])
				{
					rate +=1.0/(k+1);
					break;
				}
			}
		}
		avg+=rate;
	}
	return avg/nq;
}

float compute_number_closer(int** gnds, int** indices,int nq,int nn,int* nums)
{
	float avg = 0;
	for (size_t i=0; i<nq; ++i) 
	{
		float rate=0.0;
		for(int j=0;j<nn;j++)
		{
			for(int k=0;k<nums[i];k++)
			{
				if(gnds[i][j]==indices[i][k])
				{
					rate +=(float)(j+1)/(k+1);
					break;
				}
			}
		}
		avg+=rate/nn;
	}
	return avg/nq;
}


float compute_relative_distance_error(int** gnds,DistData** points,DistData** query, float** distances,int nq,int nn,int dim,int* nums)
{
	float count = 0;
	for (size_t i=0; i<nq; ++i) 
	{
		float sum=0.0;
		if(nums[i]!=0)
		{
			for (size_t j=0;j<nums[i];++j)
			{
				float min_distance= points[gnds[i][j]]-> distanceTo(query[i]);
				float test_distance=distances[i][j];
				float d= (test_distance - min_distance )/min_distance;
				if(d >4)
					sum +=4;
				else
					sum +=d;
			}
			sum = sum /nums[i];
			count += sum ;
		}

	}
	return count/ nq;
}

float compute_mean_average_precision (int** gnds,  int** indices,int nq,int nn,int* nums)
{
	float sum = 0;
	for (int i = 0; i < nq; ++i) 
	{
		float rate = 0.0;
		int re_n = 0;
		int count = 0;
		for(int gs_n=0; gs_n < nums[i]; gs_n++)
		{
			if(gnds[i][gs_n]==indices[i][re_n])
			{
				count++;
				rate += 1.0*(count)/(gs_n+1);
				re_n ++;
			}
		}
		sum += rate/nn;
	}
	return sum / nq;
}

float compute_discounted_culmulative_gain (int** gnds,  int** indices,int nq,int nn,int* nums) 
{
	float sum = 0;
	for (int i = 0; i < nq; ++i) 
	{
		float rate = 0.0;
		int re_n = 0;
		for(int gs_n=0; gs_n < nums[i]; gs_n++)
		{
			if(gnds[i][gs_n]==indices[i][re_n])
			{
				rate += 1.0/log2(re_n+2);
				re_n ++;
			}
		}
        sum += rate;
    }
    return sum / nq;
}

#endif

