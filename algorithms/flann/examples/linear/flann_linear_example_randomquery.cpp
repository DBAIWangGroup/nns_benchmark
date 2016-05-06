
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <getopt.h>

#include <fstream>
#include <stdio.h>


using namespace flann;
using namespace std;
using std::endl;
using std::ofstream;

int main(int argc, char** argv)
{
   
	const struct option longopts[] ={
    {"help",                        no_argument,       0, 'h'},
    {"file_path",                   required_argument, 0, 'f'},
    {"data_name",                   required_argument, 0, 'd'},
    {"nn",                          required_argument, 0, 'k'},
  };
  	int index;
  	int iarg = 0;
	char file_path[20];
	char data_name[10];
	int nn;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:d:k:h",longopts, &index);

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

	char data_path[100];
	sprintf(data_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/data/dataset.hdf5",file_path,nn);
    load_from_file(dataset, data_path ,"dataset");
    load_from_file(query, data_path ,"randomquery");
    
    ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/exp_fig/result/Linear_randomquery_%dk.txt",file_path,nn);
    out.open(result_path );

    //build index
    Index<L2<float> > index(dataset, flann::LinearIndexParams());
    index.buildIndex();
    out<<"#Algorithm = Linear"<<endl;
                                                                                            
    // do a knn search, using 128 checks
    
    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    clock_t start_time_=clock();
    index.knnSearch(query, indices, dists, nn, flann::SearchParams());
    double search_time = double(clock()-start_time_)/CLOCKS_PER_SEC;
    
	out<< search_time/query.rows <<" #search_time"<<endl;

	char indices_path[100];
	sprintf(indices_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/gnd/Linear_randomquery_%dk.hdf5",file_path,nn);
    flann::save_to_file(indices,indices_path ,"result");
    delete[] indices.ptr();
    delete[] dists.ptr();
    
    delete[] dataset.ptr();
    delete[] query.ptr();
    }
  
    
    return 0;
}


