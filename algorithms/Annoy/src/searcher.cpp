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

static const unsigned LSHKIT_HEADER = 3;

typedef float T;
typedef unsigned S;

template<typename P>
P* load_data(const char* data_path, int* len, int* d)
{
	// start to read data 
	FILE * dfp = fopen(data_path, "rb");
	int dim;
	fread(&dim,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(P)*dim));
	*d = dim;
	*len = n;
	P * data = new P[n*dim];
	for (int i=0;i<n;i++)
	{
		fread(&dim, sizeof(int),1,dfp);
		fread(&data[i*dim], sizeof(P), dim, dfp);
	}
	fclose(dfp);
    return data;
}

T get_recall(S * gt, vector<int32_t>* res, S k)
{
	float ratio=0;
	std::set<S> gnd_row;
	for(unsigned j=0; j<k; j++)
		gnd_row.insert(gt[j]);
	for(unsigned j=0; j<k; j++)
	{
		if(gnd_row.find(res->at(j))!=gnd_row.end())
			ratio++;
	}
	return ratio/k;
}


T get_mAP(S * gt, vector<int32_t>* res, S k)
{
	std::set<S> gs_set;
	int found_last=0;
	float mAP=0.0;
	for(unsigned j=0;j<k;j++)
	{
		gs_set.insert(gt[j]);
		int count = 0;
		for(unsigned t=0;t<=j;t++)
			if(gs_set.find(res->at(t))!=gs_set.end())
			{
				count++;
			}
		mAP += count*(count-found_last)*1.0/(j+1);
		found_last=count;
	}
    return mAP/k;
}

int main(int argc, char *argv[]){

	const struct option longopts[] ={
	    {"help",                        no_argument,       0, 'h'},
	    {"data_path",                   required_argument, 0, 'i'},
	    {"query_path" ,                 required_argument, 0, 'q'},
	    {"gnd_path",                    required_argument, 0, 'g'},
		{"output_path",                 required_argument, 0, 'o'},
		{"k",                           required_argument, 0, 'k'},
		{"c",                           required_argument, 0, 'c'},
	  };
	  int ind;
	  int iarg = 0;
	  opterr = 1;

	  char gnd_path[256] = "";
	  char query_path[256] = "";
	  char index_path[256] = "";
	  char output_path[256]= "";
	  int K;
	  int nStop;

	  while (iarg != -1) {
	    iarg = getopt_long(argc, argv, "i:q:g:o:k:c:h",
	                       longopts, &ind);

	    switch (iarg) {
	      case 'i':
	        if (optarg) {
	        	strcpy(index_path, optarg);
	        }
	        break;
	      case 'q':
	        if (optarg) {
	        	strcpy(query_path, optarg);
	        }
	        break;
	      case 'g':
	        if (optarg) {
	        	strcpy(gnd_path, optarg);
	        }
	        break;
	      case 'o':
	        if (optarg) {
	        	strcpy(output_path, optarg);
	        }
	        break;
		  case 'k':
	    	  if (optarg) {
	    	  K = atoi(optarg);
	    	  }
	    	  break;
		  case 'c':
	    	  if (optarg) {
			  nStop = atoi(optarg);
	    	  }
	    	  break;
	      }
	}
	
    int dim, nq;
    T *query_data = load_data<T>(query_path, &nq,&dim);
	S *gnd_data = load_data<S>(gnd_path, &nq, &K);

	AnnoyIndex<int32_t, float, Euclidean, Kiss64Random>* index = new AnnoyIndex<int32_t, float, Euclidean, Kiss64Random>(dim);
	index->load(index_path);	

	assert( query_data != NULL );
	T recall=0;
	int search_N=0;
	T mAP =0;
	timeval start;
	timeval end;
	T search_time=0;
	FILE *ofp = fopen(output_path, "a+");

	for ( int i=0; i<nq; i++ ){
		vector<int> result;
		vector<T> distance;
        gettimeofday(&start, NULL);
		//index->get_nns_by_vector ((T*) (query_data +i*dim*sizeof(T)), K, nStop, &result, &distance);
		index->get_nns_by_vector (&query_data[i*dim], K, nStop, &result, &distance);
		gettimeofday(&end, NULL);
	    search_time += diff_timeval(end, start);
		search_N += result[K];
		recall += get_recall(&gnd_data[i*K],&result,K);
		
		mAP += get_mAP(&gnd_data[i*K],&result,K);
	}
	
	recall /= nq;
	search_N /= nq;
	mAP /= nq;
	search_time /= nq;
	fprintf(ofp,"%.6f %.6f %.6f #N_%d %.6f \n",recall,0.0,search_time,search_N,mAP);	
	fclose(ofp);

	if ( query_data != NULL ) delete []query_data;

	delete index;
}
