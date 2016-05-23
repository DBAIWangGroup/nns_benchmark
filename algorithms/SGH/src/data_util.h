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
//#include "Params.h"

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;

void transpose(vector<vector<float> > X,vector<vector<float> >* Y)
{

	int dim=X.size();
	int n=X[0].size();

	for(int i=0;i<n;i++)
	{
		vector<float> y_;
		for(int j=0;j<dim;j++)
		{
			y_.push_back(X[j][i]);
		}
		Y->push_back(y_);
	}
}


void ReadPoints(char* filepath,vector<vector<float> >* points, int* len, int* d)
{
	FILE * dfp = fopen(filepath, "rb");
	int dim;
	fread(&dim,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(float)*dim));
	*d = dim;
	*len = n;
	points->resize(n);
	for (int i=0;i<n;i++)
	{
		fread(&dim, sizeof(int),1,dfp);
		points->at(i).resize(dim);
    	for(int j = 0; j < dim; ++j) {
    		float buffer;
			fread(&(buffer), sizeof(float),1,dfp);
    		points->at(i)[j] = buffer;
    	}
	}
}

void ReadGroundtruth(char* filepath, vector<vector<int> >* gnds, int* len, int* d)
{
	FILE * dfp = fopen(filepath, "rb");
	int k;
	fread(&k,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(int)*k));
	*d = k;
	*len = n;
	gnds->resize(n);
	for (int i=0;i<n;i++)
	{
		fread(&k, sizeof(int),1,dfp);
		gnds->at(i).resize(k);
    	for(int j = 0; j < k; ++j) {
    		int buffer;
			fread(&(buffer), sizeof(int),1,dfp);
    		gnds->at(i)[j] = buffer;
    	}
	}
}

void ReadB1(const string& filename, vector<vector<unsigned> >* B1,int n) {//
  ifstream input;
	input.open(filename.c_str(), ios::binary);
  if(!input.good()) {
    throw std::logic_error("Invalid filename");
  }
  B1->resize(n);
  int cbits;
  for(int pid = 0; pid < n; ++pid) {
    input.read((char*)&cbits, sizeof(cbits));
    if(cbits <= 0) {
      throw std::logic_error("Bad file content: non-positive dimension");
    }
    B1->at(pid).resize(cbits);
    for(int d = 0; d < cbits; ++d) {
      int buffer;
      input.read((char*)&(buffer), sizeof(int));
      B1->at(pid)[d] = (unsigned)buffer;//(unsigned)
    }
  }
}


void ReadChecks(const string& filename, vector<int>* checks) {
  FILE *fp = fopen(filename.c_str(), "r");
  int n;
  fscanf(fp, "%d ", &n);
  checks->resize(n);
  int tmp;
  for (int i = 0; i < n; ++i) {
    fscanf(fp, "%d ", &tmp);
    checks->at(i)=tmp;
  }
}





