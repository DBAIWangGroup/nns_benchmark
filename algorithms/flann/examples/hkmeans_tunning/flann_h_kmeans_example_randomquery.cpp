
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

float compute_precision(const flann::Matrix<int>& match, const flann::Matrix<int>& indices)
{
	int count = 0;

	assert(match.rows == indices.rows);
	size_t nn = std::min(match.cols, indices.cols);

	for (size_t i=0; i<match.rows; ++i) {
		for (size_t j=0;j<nn;++j) {
			for (size_t k=0;k<nn;++k) {
				if (match[i][j]==indices[i][k]) {
					count ++;
				}
			}
		}
	}

	return float(count)/(nn*match.rows);
}

float compute_recall_at(const size_t nn, const flann::Matrix<int>& match, const flann::Matrix<int>& indices)
{
	int count = 0;

	assert(match.rows == indices.rows);
	//size_t nn = match.cols; //std::min(match.cols, indices.cols);

	for (size_t i=0; i<match.rows; ++i) 
	{
		for (size_t j=0;j<nn;++j) 
		{
			if (match[i][j]==indices[i][0]) 
			{
				count ++;
				break;
			}
		}
	}

	return float(count)/(match.rows);
}

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char** argv)
{

	const struct option longopts[] ={
    {"help",                        no_argument,       0, 'h'},
    {"file_path",                   required_argument, 0, 'f'},
    {"data_name",                   required_argument, 0, 'd'},
    {"trees",                       required_argument, 0, 't'},
    {"nn",                          required_argument, 0, 'k'},
  };
  	int index;
  	int iarg = 0;
	char file_path[20];
	char data_name[10];
	int branches;
	int nn;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:d:k:b:h",longopts, &index);

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
      case 'b':
        if (optarg) {
          branches = atoi(optarg);
        }
        break;
    }
	}

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

	char data_path[100];
	sprintf(data_path,"/home/liwen/Downloads/Dataset/%s/data/dataset_randomquery.hdf5",file_path);
    load_from_file(dataset, data_path,"dataset");
    load_from_file(query, data_path,"query");
    load_from_file(groundtruth,data_path,"groundtruth");
    
    //int branches[5]={10,20,32,64,128};
	//for(int b=0;b<5;b++)
	{
    ofstream out_recall;
	char result_path_recall[100];
	sprintf(result_path_recall,"/home/liwen/Downloads/Dataset/%s/exp_fig/result/Flann/flann_HKMeans_%dbranches_recall_randomquery.txt",file_path,branches);
    out_recall.open(result_path_recall);

	ofstream out_precision;
	char result_path_precision[100];
	sprintf(result_path_precision,"/home/liwen/Downloads/Dataset/%s/exp_fig/result/Flann/flann_HKMeans_%dbranches_precision_randomquery.txt",file_path,branches);
    out_precision.open(result_path_precision);

    //build index
    Index<L2<float> > index(dataset, flann::KMeansIndexParams(branches,10,FLANN_CENTERS_RANDOM ,0.4));
    clock_t start_time_=clock();
    index.buildIndex();
    double index_time = double(clock()-start_time_)/CLOCKS_PER_SEC;
    
	out_recall<<"#Algorithm = H_KMeans  Centroids_count ="<<branches<<" iterations=10 cb_index = 0.4"<<endl;
    out_recall<< index_time <<" #index_time "<<endl;

	out_precision<<"#Algorithm = H_KMeans  Centroids_count ="<<branches<<" iterations=10 cb_index = 0.4"<<endl;
    out_precision<< index_time <<" #index_time "<<endl;
            
	char index_path[100];
	sprintf(index_path, "hkmeans/%s_HKMeans_indices_%dbranches_randomquery",data_name,branches);
                                                                              
    index.save(index_path); 

    // do a knn search, using 128 checks
    int checks[15]={100,200,300,500,700,1000,1500,2000,3000,5000,7000,10000,15000,20000,30000}; 
    for(int c=0;c<15;c++)
    {
    std::cout<<c<<endl; 
    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    start_time_=clock();
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(checks[c]));
    double search_time = double(clock()-start_time_)/CLOCKS_PER_SEC;

    //compute the recall 
    //std::ofstream out("result.txt");
    
    float precision = compute_precision(indices,groundtruth);
	out_precision.setf(ios::fixed);
	out_precision<< precision <<" "<< search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
	
	
	double recall[5]={0.0,0.0,0.0,0.0,0.0};
	recall[0]= compute_recall_at(1,indices,groundtruth);
    recall[1]= compute_recall_at(10,indices,groundtruth);
    recall[2]= compute_recall_at(100,indices,groundtruth);
    recall[3]= compute_recall_at(1000,indices,groundtruth);
    recall[4]= compute_recall_at(10000,indices,groundtruth);
	out_recall.setf(ios::fixed);
	out_recall << recall[0] << " " << recall[1] << " " << recall[2] << " " << recall[3] << " " << recall[4] << " " << search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
    

	char indices_path[100];
	sprintf(indices_path,"/home/liwen/Downloads/Dataset/%s/result/hkmeans_result_%dbranches_randomquery.hdf5",file_path,branches);
    flann::save_to_file(indices,indices_path,"result");

    delete[] indices.ptr();
    delete[] dists.ptr();
    }
    }

    delete[] dataset.ptr();
    delete[] query.ptr();
    
  
    
    return 0;
}


