
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <getopt.h>
#include <sys/time.h>
#include <fstream>
#include <stdio.h>


using namespace flann;
using namespace std;
using std::endl;
using std::ofstream;

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}


int main(int argc, char** argv)
{
   
	const struct option longopts[] ={
    {"help",                        no_argument,       0, 'h'},
    {"file_path",                   required_argument, 0, 'f'},
    {"data_name",                   required_argument, 0, 'd'},
	{"pre_name",                    required_argument, 0, 'p'},
	{"suf_name",                    required_argument, 0, 's'},
    {"nn",                          required_argument, 0, 'k'},
  };
  	int index;
  	int iarg = 0;
	char file_path[20];
	char data_name[10];
	char pre_name[10];
	char suf_name[10];
	int nn;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:d:k:p:s:h",longopts, &index);

    switch (iarg) {
      case 'f':
        if (optarg) {
			strcpy(file_path, optarg);
        }
        break;
      case 'd':
        if (optarg) {
			strcpy(data_name, optarg);
        }
        break;
      case 'p':
        if (optarg) {
			strcpy(pre_name, optarg);
        }
        break;
	case 's':
        if (optarg) {
			strcpy(suf_name, optarg);
        }
        break;
      case 'k':
        if (optarg) {
			nn = atoi(optarg);
        }
        break;
    }
	}

	{
    Matrix<float> dataset;
    Matrix<float> query;
	cout<<nn<<endl;
	char data_path[100];
	sprintf(data_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/data/%sdataset%s.hdf5",file_path,pre_name,suf_name);
    load_from_file(dataset, data_path ,"dataset");
    load_from_file(query, data_path ,"query");
    
    ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/exp_fig/result/%sLinear%s_%dk.txt",file_path,pre_name,suf_name,nn);
    out.open(result_path );

    //build index
    Index<L2<float> > index(dataset, flann::LinearIndexParams());
    index.buildIndex();
    out<<"#Algorithm = Linear"<<endl;
                                                                                            
    // do a knn search, using 128 checks
    
    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

	timeval start;
    gettimeofday(&start, NULL);
    index.knnSearch(query, indices, dists, nn, flann::SearchParams());
	timeval end;
    gettimeofday(&end, NULL);
    double search_time = diff_timeval(end, start);
    
	out<< search_time/query.rows <<" #search_time"<<endl;
//*
	char indices_path[100];
	sprintf(indices_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/gnd/%sLinear%s_%dk.hdf5",file_path,pre_name,suf_name,nn);
    flann::save_to_file(indices,indices_path ,"result");//*/

/*
	ofstream out;
	char result_path[100];
	sprintf(result_path,"./gnd/%s.txt",data_name);
    out.open(result_path );
	out<<dists.rows<<" "<<dists.cols<<endl;
	cout<<indices[0][0]<<endl;
	for(int i=0;i<dists.rows;i++)
	{
		for(int j=0;j<dists.cols;j++)
		{
			out<<sqrt(dists[i][j])<<" ";
		}
		out<<"\n";
	}*/

    delete[] indices.ptr();
    delete[] dists.ptr();
    
    delete[] dataset.ptr();
    delete[] query.ptr();
    }
  
    
    return 0;
}


