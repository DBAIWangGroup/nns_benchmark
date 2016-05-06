
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>


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


int main(int argc, char** argv)
{
    int nn = 100;

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

    load_from_file(dataset, "/home/liwen/Downloads/Dataset/Audio/data/dataset.hdf5","dataset");
    load_from_file(query, "/home/liwen/Downloads/Dataset/Audio/data/dataset.hdf5","query");
    load_from_file(groundtruth,"/home/liwen/Downloads/Dataset/Audio/data/dataset.hdf5","groundtruth");
    
    int branches[1]={64};
	for(int b=0;b<1;b++)
	{

    //build index
    Index<L2<float> > index(dataset, flann::KMeansIndexParams(branches[b],10,FLANN_CENTERS_RANDOM ,0.4));
    clock_t start_time_=clock();
    index.buildIndex();
    double index_time = double(clock()-start_time_)/CLOCKS_PER_SEC;
            

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
	
	int mem=index.usedMemory();
    cout<<mem<<endl;
    delete[] indices.ptr();
    delete[] dists.ptr();
    }
    }

    delete[] dataset.ptr();
    delete[] query.ptr();
    
  
    
    return 0;
}


