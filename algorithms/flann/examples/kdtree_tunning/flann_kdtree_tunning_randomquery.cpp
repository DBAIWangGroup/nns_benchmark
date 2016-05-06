
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <sys/time.h>
#include <getopt.h>

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
	int trees;
	int nn;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:d:k:t:h",longopts, &index);

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
      case 't':
        if (optarg) {
          trees = atoi(optarg);
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

    {
    ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/liwen/Downloads/Dataset/%s/exp_fig/result/Flann/flann_KDTree_%dtrees_precision_randomquery.txt",file_path,trees);
    out.open(result_path);

	ofstream out_recall;
	char result_path_recall[100];
	sprintf(result_path_recall,"/home/liwen/Downloads/Dataset/%s/exp_fig/result/Flann/flann_KDTree_%dtrees_recall_randomquery.txt",file_path,trees);
    out_recall.open(result_path_recall);

    //build index
    Index<L2<float> > index(dataset, flann::KDTreeIndexParams(trees));
	clock_t start_time_=clock();
    index.buildIndex();
    double index_time = double(clock()-start_time_)/CLOCKS_PER_SEC;

    out<<"#Algorithm = KDTree trees="<<trees<<" "<<endl;
    out<< index_time <<" #index_time "<<endl;

	out_recall<<"#Algorithm = KDTree trees="<<trees<<" "<<endl;
    out_recall<< index_time <<" #index_time "<<endl;
	
	char index_path[100];
    sprintf(index_path,"kdtree/%s_KDTree_indices_%dtrees_randomquery",data_name,trees);                                                             
    index.save(index_path); 

    //do a knn search, using 128 checks
    int checks[15]={100,200,300,500,700,1000,1500,2000,3000,5000,7000,10000,15000,20000,30000}; 
    for(int c=0;c<15;c++)
    {
    std::cout<<c<<endl; 
    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    start_time_=clock();
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(checks[c]));
    double search_time = double(clock()-start_time_)/CLOCKS_PER_SEC;
   
    float precision = compute_precision(indices,groundtruth);
	out.setf(ios::fixed);
	out<< precision <<" "<< search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
	
	
	double recall[5]={0.0,0.0,0.0,0.0,0.0};
	recall[0]= compute_recall_at(1,indices,groundtruth);
    recall[1]= compute_recall_at(10,indices,groundtruth);
    recall[2]= compute_recall_at(100,indices,groundtruth);
    recall[3]= compute_recall_at(1000,indices,groundtruth);
    recall[4]= compute_recall_at(10000,indices,groundtruth);
	out_recall.setf(ios::fixed);
	out_recall << recall[0] << " " << recall[1] << " " << recall[2] << " " << recall[3] << " " << recall[4] << " " << search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
    

	char indices_path[100];
	sprintf(indices_path,"/home/liwen/Downloads/Dataset/%s/result/kdtree_result_%dtrees_randomquery.hdf5",file_path,trees);
    flann::save_to_file(indices,indices_path,"result");

    delete[] indices.ptr();
    delete[] dists.ptr();
    }
    }

    delete[] dataset.ptr();
    delete[] query.ptr();
    
  
    
    return 0;
}


