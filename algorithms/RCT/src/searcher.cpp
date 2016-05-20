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


//int maxChecks;
float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char * argv[]) {
	const struct option longopts[] ={
	    {"help",                        no_argument,       0, 'h'},
	    {"dataset_filepath",            required_argument, 0, 's'},
	    {"query_filepath" ,             required_argument, 0, 'q'},
	    {"groundtruth_filepath",        required_argument, 0, 'g'},
	    {"indices_filepath",            required_argument, 0, 'i'},
	    {"output_filepath",             required_argument, 0, 'o'},
	    {"n",                           required_argument, 0, 'n'},
	    {"m",                           required_argument, 0, 'm'},
		{"k",                           required_argument, 0, 'k'},
		{"c",                           required_argument, 0, 'c'},
	  };
	  int index;
	  int iarg = 0;
	  opterr = 1;    //getopt error message (off: 0)
      int n;
	  int nq;
      int dim;
      int nn;
      float scale;

	  char groundtruth_filepath[100] = "";
	  char query_filepath[100] = "";
	  char dataset_filepath[100] = "";
	  char output_filepath[100] = "";  //output file, currently not used.
      char indices_filepath[100] = "";

	  while (iarg != -1) {
	    iarg = getopt_long(argc, argv, "s:q:g:i:o:n:m:k:c:h",
	                       longopts, &index);

	    switch (iarg) {
	      case 's':
	        if (optarg) {
	        	strcpy(dataset_filepath, optarg);
	        }
	        break;
	      case 'q':
	        if (optarg) {
	        	strcpy(query_filepath, optarg);
	        }
	        break;
	      case 'g':
	        if (optarg) {
	        	strcpy(groundtruth_filepath, optarg);
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
	      case 'k':
	    	  if (optarg) {
	    	  nn = atoi(optarg);
	    	  }
	    	  break;
		  case 'c':
	    	  if (optarg) {
 				scale = atof(optarg);
	    	  }
	    	  break;
	      }
	  }
	  //read data points
	  DistData** points = ReadPoints(dataset_filepath, &n, &dim);

	  //read query
	  DistData** query = ReadPoints(query_filepath, &nq, &dim);

      //read groundtruth
      int** gnd=new int*[nq];
      ReadGroundtruth(groundtruth_filepath, gnd, nq);

	  ofstream out;
	  out.open(output_filepath,ios::app);
	  out.setf(ios::fixed);

	  RCT* rct = new RCT();
	  rct->build(indices_filepath, points, n);
	  
	  timeval start;
      gettimeofday(&start, NULL);
  
      int** indices = new int* [nq];
	  float** distances = new float* [nq];
	  int* nums=new int[nq];
	  int checks = 0;
	  for(int i=0;i<nq;i++)
	  {
		indices[i]=new int[nn];
		distances[i] = new float[nn];			
	    int num = rct->findNear(query[i], nn,scale);
		nums[i]=num;
 	    rct->getResultIndices(indices[i], nn);
		rct->getResultDists(distances[i], nn);
		checks+=rct->checks;
	  }
	  timeval end;
      gettimeofday(&end, NULL);
      float search_time = diff_timeval(end, start)/nq;

      float recall = compute_recall(gnd, indices,nq,nn,nums);

	  out<<recall<<" "<<search_time<<" #N_"<<checks/nq<<" "<<endl;
}

