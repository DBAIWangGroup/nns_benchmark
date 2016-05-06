
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
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
    int nn = 100;

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

    load_from_file(dataset, "/home/liwen/Downloads/Dataset/Trevi/data/dataset.hdf5","dataset");
    load_from_file(query, "/home/liwen/Downloads/Dataset/Trevi/data/dataset.hdf5","query");
    load_from_file(groundtruth,"/home/liwen/Downloads/Dataset/Trevi/data/dataset.hdf5","groundtruth");
    
	int trees[6]={1,2, 4 ,8 ,12 ,16};
	for(int t=0;t<6;t++)
    {
    ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/liwen/Downloads/Dataset/Trevi/exp_fig/result/Flann/flann_KDTree_%dtrees.txt",trees[t]);
    out.open(result_path);

    //build index
    Index<L2<float> > index(dataset, flann::KDTreeIndexParams(trees[t]));
	clock_t start_time_=clock();
    index.buildIndex();
    double index_time = double(clock()-start_time_)/CLOCKS_PER_SEC;

    out<<"#Algorithm = KDTree trees="<<trees<<" "<<endl;
    out<< index_time <<" #index_time "<<endl;
	
	char index_path[100];
    sprintf(index_path,"kdtree/trevi_KDTree_indices_%dtrees",trees[t]);                                                             
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

    delete[] indices.ptr();
    delete[] dists.ptr();
    }
    }

    delete[] dataset.ptr();
    delete[] query.ptr();
    
  
    
    return 0;
}


