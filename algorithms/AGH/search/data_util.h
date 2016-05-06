/*
 * data_util.cpp
 *
 *  Created on: 23/09/2015
 *      Author: liwen
 */
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
	cout<<"n:"<<n<<endl;

	for(int i=0;i<n;i++)
	{
		vector<float> y_;
		for(int j=0;j<dim;j++)
		{
			y_.push_back(X[j][i]);
		}
		Y->push_back(y_);
	}
	cout<<"X01:"<<X[0][1]<<endl;
	cout<<"Y10"<<Y->at(1)[0]<<endl;
}

void ReadPoints(const string& filename, vector<vector<float> >* points, int count, int *dim) {
	//vector<vector<float> > X;
	ifstream input;
	input.open(filename.c_str(), ios::binary);
	if(!input.good()) {
		throw std::logic_error("Invalid filename");
	}
	//X.resize(count);
	points->resize(count);
	int dimension;
	for(int pid = 0; pid < count; ++pid) {
		input.read((char*)&dimension, sizeof(dimension));
		if(dimension <= 0) {
			throw std::logic_error("Bad file content: non-positive dimension");
		}
	//X.at(pid).resize(dimension);
	points->at(pid).resize(dimension);
    for(int d = 0; d < dimension; ++d) {
    	float buffer;
    	input.read((char*)&(buffer), sizeof(float));
    	//X.at(pid)[d] = buffer;
    	points->at(pid)[d] = buffer;
    }
  }
  *dim = dimension;
  //points = &X;
  //transpose(X,points);
}

void ReadGroundtruth(const string& filename, vector<vector<int> >* gnds,int nq, int nn) {
  ifstream input;
	input.open(filename.c_str(), ios::binary);
  if(!input.good()) {
    throw std::logic_error("Invalid filename");
  }
  gnds->resize(nq);
  int k;
  for(int pid = 0; pid < nq; ++pid) {
    input.read((char*)&k, sizeof(k));
    if(k <= 0) {
      throw std::logic_error("Bad file content: non-positive dimension");
    }
    gnds->at(pid).resize(k);
    for(int d = 0; d < k; ++d) {
      int buffer;
      input.read((char*)&(buffer), sizeof(int));
      gnds->at(pid)[d] = buffer;
    }
  }
  //*nn = k;
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





