
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
float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char** argv)
{
    int nn = 100;

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

    load_from_file(dataset, "/home/liwen/Downloads/Dataset/SUN397/data/dataset.hdf5","dataset");
    load_from_file(query, "/home/liwen/Downloads/Dataset/SUN397/data/dataset.hdf5","query");
    load_from_file(groundtruth,"/home/liwen/Downloads/Dataset/SUN397/data/dataset.hdf5","groundtruth");
    
	double target_pre[2]={0.7,0.95};
    for (int i=0;i<2;i++)
	{
    

	double target_precision=target_pre[i];
	
	double build_w[6]={0,0,0.01,0.01,1,1};
	double memory_w[6]={0,1,0,1,0,1};
	for (int j=0;j<6;j++)
	{
	double build_weight=build_w[j];
	double memory_weight=memory_w[j];
	
	ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/liwen/Downloads/Dataset/SUN397/exp_fig/result/Flann/flann_Auto_%.2fPrecision_%.2fBuild_%.0fMemory.txt",target_precision,build_weight,memory_weight);
    out.open(result_path);
	/*
	ofstream out_recall;
	char recall_path[100];
	sprintf(recall_path,"/home/liwen/Downloads/Dataset/Audio/exp_fig/result/Flann/flann_Auto_recall_%.2fPrecision_%.2fBuild_%.0fMemory.txt",target_precision,build_weight,memory_weight);

    out_recall.open(recall_path);
    */
	//build index	
	double sample_fraction=0.1;
    Index<L2<float> > index(dataset, flann::AutotunedIndexParams(target_precision, build_weight, memory_weight, sample_fraction));
	cout<<"sun397 target:"<<target_precision<<" build_weight:"<<build_weight<<" memory_weight:"<<memory_weight<<" \n";
    clock_t start_time_=clock();
    index.buildIndex();
    double index_time = double(clock()-start_time_)/CLOCKS_PER_SEC;

    out<<"#Algorithm = Auto  target_precision = 0.95, build_weight = 0.01, memory_weight = 0, sample_fraction = 0.1 "<<endl;
    out<< index_time <<" #index_time "<<endl;

	char index_path[100];
	sprintf(index_path,"auto/sun397_Auto_indices_%.2fPrecision_%.2fBuild_%.0fMemory",target_precision,build_weight,memory_weight);                                                                       
    index.save(index_path); 
  
    // do a knn search, using 128 checks
    int checks[15]={100,200,300,500,700,1000,1500,2000,3000,5000,7000,10000,15000,20000,30000}; 
    for(int c=0;c<15;c++)
    {
    //std::cout<<c<<endl;

    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    start_time_=clock();
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(checks[c]));
    double search_time = double(clock()-start_time_)/CLOCKS_PER_SEC;

    //compute the recall 
    
    float precision = compute_precision(indices,groundtruth);
	out.setf(ios::fixed);
	out<< precision <<" "<< search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
	
	//float recall = compute_recall_at(1,indices,groundtruth);
	/*
	double recall[5]={0.0,0.0,0.0,0.0,0.0};
    recall[0]= compute_recall_at(1,indices,groundtruth);
    recall[1]= compute_recall_at(10,indices,groundtruth);
    recall[2]= compute_recall_at(100,indices,groundtruth);
    recall[3]= compute_recall_at(1000,indices,groundtruth);
    recall[4]= compute_recall_at(10000,indices,groundtruth);
	out_recall.setf(ios::fixed);
	out_recall << recall[0] << " " << recall[1] << " " << recall[2] << " " << recall[3] << " " << recall[4] << " " << search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
*/
 	char indices_path[100];
	sprintf(indices_path,"/home/liwen/Downloads/Dataset/SUN397/result/Auto_result_%.2fPrecision_%.2fBuild_%.0fMemory.hdf5",target_precision,build_weight,memory_weight);
    flann::save_to_file(indices,indices_path,"result");   
    delete[] indices.ptr();
    delete[] dists.ptr();
    }
    }
	}

    delete[] dataset.ptr();
    delete[] query.ptr();
    
  
    
    return 0;
}


