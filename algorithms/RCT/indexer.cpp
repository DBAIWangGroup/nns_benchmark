/*
 * m.cpp
 *
 *  Created on: 01/10/2015
 *      Author: liwen
 */

#include "data_util.h"
#include "rct.h"
#include <stdlib.h>
#include <cstring>
#include <malloc.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <getopt.h>
#include <cstdlib>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <stdexcept>

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;


float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char * argv[]) {
	const struct option longopts[] ={
	    {"help",                        no_argument,       0, 'h'},
	    {"dataset_filepath",            required_argument, 0, 's'},
	    {"indices_filepath",            required_argument, 0, 'i'},
		{"output_filepath",             required_argument, 0, 'o'},
	    {"n",                           required_argument, 0, 'n'},
	    {"maxParents",                  required_argument, 0, 'p'},
	    {"scaleFactor",                 required_argument, 0, 'f'},
		{"sampleRate",                  required_argument, 0, 'r'},
	  };
	  int index;
	  int iarg = 0;
	  opterr = 1;    //getopt error message (off: 0)
	  int n;   //data size
	  int dim;  //dimension
	  int maxParents; //max parent size
	  float scaleFactor; 
	  int r; //for show

	  char dataset_filepath[100] = "";
      char indices_filepath[100] = "";
      char output_filepath[100] = "";
	  while (iarg != -1) {
	    iarg = getopt_long(argc, argv, "s:i:o:n:p:f:r:h",
	                       longopts, &index);

	    switch (iarg) {
	      case 's':
	        if (optarg) {
	        	strcpy(dataset_filepath, optarg);
	        }
	        break;
		  case 'i':
	        if (optarg) {
	        	strcpy(indices_filepath, optarg);
	        }
	        break;
		  case 'o':
	        if (optarg) {
	        	strcpy(output_filepath, optarg);
	        }
	        break;
	      case 'n':
	    	  if (optarg) {
	    	  n = atoi(optarg);
	    	  }
	    	  break;
		  case 'p':
	    	  if (optarg) {
	    	  maxParents = atoi(optarg);
	    	  }
	    	  break;
		  case 'f':
	    	  if (optarg) {
	    	  scaleFactor = atof(optarg);
	    	  }
	    	  break;
		  case 'r':
	    	  if (optarg) {
	    	  r = atof(optarg);
	    	  }
	    	  break;
	      }
	  }
	  //read data points
	  DistData** points = new DistData*[n];
	  ReadPoints(dataset_filepath,points, n, &dim);

	  //build rct
	  timeval start;
      gettimeofday(&start, NULL);
	  
	  RCT* rct = new RCT();
	  rct->setVerbosity(2);
	  const float sampleRate_ = pow(n, 1.0 / r);
	  rct->setSampleRate(sampleRate_);
	  rct->build(points, n, scaleFactor, maxParents);

      timeval end;
      gettimeofday(&end, NULL);
      float build_time = diff_timeval(end, start);
	   cout<<"finish build"<<endl;
	  int levels = rct->getNumLevels();

	  ofstream out;
	  out.open(output_filepath,ios::app);
	  out.setf(ios::fixed);
	  out<<build_time << " #levels:"<<levels<<" #sample_rate:"<<r << " #maxParent:"<<maxParents<<" #scale:"<< scaleFactor <<endl;
	  int return_size = rct->saveToFile(indices_filepath);
     cout<<"finish all"<<endl;
}

