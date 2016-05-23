#include "data_util.h"
#include "SGH.h"
#include "hammingDist.h"
#include "eval.h"

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

vector<vector <float> > points;
vector<vector <float > > query;
vector<vector <int> > gnds;
//vector<int> maxChecks;

int n;
int nq;
int dim;
int k;
int radis;
int sample_size;

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char * argv[]) {
	const struct option longopts[] ={
	    {"help",                        no_argument,       0, 'h'},
	    {"dataset_filepath",            required_argument, 0, 's'},
	    {"query_filepath" ,             required_argument, 0, 'q'},
	    {"groundtruth_filepath",        required_argument, 0, 'g'},
		{"k",                           required_argument, 0, 'k'},
		{"c",                           required_argument, 0, 'c'},
		{"t",                           required_argument, 0, 't'},
		{"r",                           required_argument, 0, 'r'},
	    {"dataset_binary_path",         required_argument, 0, 'b'},
	    {"params_path",                 required_argument, 0, 'p'},
	  };
	  int ind;
	  int iarg = 0;
	  opterr = 1;
	  int checks;
	  char groundtruth_filepath[100] = "";
	  char query_filepath[100] = "";
	  char dataset_filepath[100] = "";
	  char dataset_binary_path[100] = "";
	  char params_path[100] = "";
	  //char checks_path[20] ="";

	  while (iarg != -1) {
	    iarg = getopt_long(argc, argv, "s:q:g:k:r:t:c:b:p:h",
	                       longopts, &ind);

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
		  case 'k':
	    	  if (optarg) {
	    	  k = atoi(optarg);
	    	  }
	    	  break;
		  case 'c':
	    	  if (optarg) {
			  checks = atoi(optarg);
			  //strcpy(checks_path, optarg);
	    	  }
	    	  break;
		  case 'r':
	    	  if (optarg) {
	    	  radis = atoi(optarg);
	    	  }
	    	  break;
	      case 'b':
	        if (optarg) {
	        	strcpy(dataset_binary_path, optarg);
	        }
	        break;
		  case 't':
	    	  if (optarg) {
	    	  sample_size = atoi(optarg);
	    	  }
	    	  break;
	      case 'p':
	        if (optarg) {
	        	strcpy(params_path, optarg);
	        }
	        break;
	        }
	      }

	  ///*
	  ReadPoints(dataset_filepath, &points, &n, &dim);

	  ReadPoints(query_filepath, &query,&nq, &dim);
      int nn;
	  ReadGroundtruth(groundtruth_filepath, &gnds, &nq, &nn);

	  //ReadChecks(checks_path,&maxChecks);

	  //read SH params
	  SGH SGH_;
	  SGH_.ReadSGHParams(params_path,sample_size, dim);
	  int nbits = SGH_.nbits;

	  vector<vector<unsigned> > B1;
	  ReadB1(dataset_binary_path,&B1,n);
	  //SH_.compressSH(&points,&B1);

	  HierarchicalClusteringIndex HCI_ ;
	  HCI_.set_params(B1,32,4,100);
	  if(radis==-2)
	  	HCI_.buildIndex();

	  timeval start;
      gettimeofday(&start, NULL);

	  // compress query
	  vector<vector<unsigned> > B2;
	  SGH_.compressSGH(&query,&B2);

	  timeval end;
	  gettimeofday(&end, NULL);
	  double transform_time = diff_timeval(end, start);

	  gettimeofday(&start, NULL);
	  vector<vector<int> > indices;
	  vector<vector<int> > dists;
	  if(radis==-1)
	    hammingDist(&B2,&B1,&indices,checks);
	  else if(radis==-2)
		HCI_.knnSearch(B2,indices,dists, checks,checks);
	  else
		hammingDist_ByRange(&B2,&B1,&indices,radis);

      		// get results after re-ranking
	  vector<vector<int> > results;
	  results = Search(&points, &query, &indices, k);
      gettimeofday(&end, NULL);
      double search_time = diff_timeval(end, start)+ transform_time ;

	  float recall = compute_recall(&gnds, &results);
      cout<<recall<<" "<< search_time/nq << " #N_" << checks <<" #bit_"<< nbits<<" #radis_"<<radis<<" "<< endl;
}




