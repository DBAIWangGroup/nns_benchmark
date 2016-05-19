#include <iostream>
#include <fstream>
#include <cctype>
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <set>
#include <getopt.h>

#include "annoy.cpp"
#include "kissrandom.h"

using namespace std;
using std::set;

struct item{

unsigned id;
};

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

//static const unsigned LSHKIT_HEADER = 3;

typedef float T;
typedef unsigned S;

float* load_data(const char* data_path, int* len, int* d)
{
	// start to read data 
	FILE * dfp = fopen(data_path, "rb");
	int dim;
	fread(&dim,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(float)*dim));
	*d = dim;
	*len = n;
	float * data = new float [n*dim];
	for (int i=0;i<n;i++)
	{
		fread(&dim, sizeof(int),1,dfp);
		fread(&data[i*dim], sizeof(float), dim, dfp);
	}
	fclose(dfp);
	return data;
}

int main(int argc, char *argv[]){

	const struct option longopts[] ={
	    {"help",                        no_argument,       0, 'h'},
	    {"data_path",                   required_argument, 0, 'd'},
		{"index_path",                  required_argument, 0, 'i'},
		{"output_path",                 required_argument, 0, 'o'},
		{"trees",                       required_argument, 0, 't'},
	  };
	  int ind;
	  int iarg = 0;
	  opterr = 1;

	  char data_path[256] = "";
	  char index_path[256] = "";
	  char output_path[256]= "";
	  int trees;

	  while (iarg != -1) {
	    iarg = getopt_long(argc, argv, "d:i:o:t:h",
	                       longopts, &ind);

	    switch (iarg) {
	      case 'i':
	        if (optarg) {
	        	strcpy(index_path, optarg);
	        }
	        break;
	      case 'd':
	        if (optarg) {
	        	strcpy(data_path, optarg);
	        }
	        break;
	      case 'o':
	        if (optarg) {
	        	strcpy(output_path, optarg);
	        }
	        break;
		  case 't':
	    	  if (optarg) {
	    	  trees = atoi(optarg);
	    	  }
	    	  break;
	      }
	}

	int dim, N;
    float *data = load_data(data_path, &N , &dim);

	AnnoyIndex<int32_t, float, Euclidean, Kiss64Random>* index = new AnnoyIndex<int32_t, float, Euclidean, Kiss64Random>(dim);
    for (unsigned i = 0; i < N; ++i) {  
        index->add_item(i, &data[i*dim]);//(T*) (data +i*dim*sizeof(T))
    }

  	timeval start;
	gettimeofday(&start, NULL);
    index->build(trees);
	timeval end;
	gettimeofday(&end, NULL);
	T index_time = diff_timeval(end, start);
	FILE *ofp = fopen(output_path, "a+");
    fprintf(ofp,"%.2f #trees_%d \n",index_time,trees);	
	fclose(ofp);

    index->save(index_path);

    delete []data;    
	delete index;

}
