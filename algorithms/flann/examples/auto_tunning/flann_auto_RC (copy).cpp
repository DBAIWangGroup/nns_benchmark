#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <getopt.h>
#include <fstream>
#include <stdio.h>


using namespace flann;
using namespace std;
using std::endl;
using std::ofstream;

float get_distance(float* point, float* query, int dim)
{
	float dist=0.0;
	for(int i=0;i<dim;i++)
	{
		float diff = (*point++ - *query++);
		dist += diff*diff;
	}
	return dist;
}

float compute_recall(int k,const flann::Matrix<int>& results, const flann::Matrix<int>& gnds)
{
	int count = 0;
	for (int i=0; i<results.rows; ++i) {
		std::set<unsigned> gnd_row;
		for(int j=0;j<k;j++)
			gnd_row.insert(gnds[i][j]);
		for(unsigned j=0;j<k;j++)
			if(gnd_row.find(results[i][j])!=gnd_row.end())
			{
				count++;
			}
	}
	return float(count)/(k*results.rows);
}

float compute_relative_distance_error(int k, const flann::Matrix<float>& dataset, const flann::Matrix<float>& query, const flann::Matrix<float>& dists, const flann::Matrix<int>& gnds) 
{
	float avg = 0.0;
    for (int i = 0; i <dists.rows; ++i) //dists.rows
	{
		float dist = 0.0;
        for(int j=0; j<k; j++)
		{
			float d_qr = dists[i][j];   //query.cols
			float d_qg = get_distance(dataset[gnds[i][j]],query[i],query.cols);
            float d=(d_qr - d_qg)/d_qg;
			if(d<=4)
				dist += d;
			else
				dist += 4; 
		}
		avg += dist;
    } 
	return avg/k/dists.rows;     
}

float compute_mean_average_precision (int k,const flann::Matrix<int>& results, const flann::Matrix<int>& gnds)
{
     float sum = 0;
     for (int i = 0; i < results.rows; ++i) {
		float rate = 0.0;
		int count = 0;
		std::set<unsigned> gnd_row;
		int found_last=0;
		for(unsigned gs_n=0; gs_n < k; gs_n++)
		{
			gnd_row.insert(gnds[i][gs_n]);
			int count=0;
			for(unsigned j=0;j<=gs_n;j++)
				if(gnd_row.find(results[i][j])!=gnd_row.end())
				{
					count++;
				}
			rate += count*(count-found_last)/(gs_n+1);
			found_last=count;
		}
        sum += rate/k;
    }
    return sum / results.rows;
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
	{"last_name",                   required_argument, 0, 'l'},
    {"wb",                          required_argument, 0, 'b'},
    {"wm",                          required_argument, 0, 'm'},
    {"tp",                          required_argument, 0, 't'},
    {"sf",                          required_argument, 0, 's'},
    {"nn",                          required_argument, 0, 'k'},
  };
	int ind;
  	int iarg = 0;
	char file_path[20];
	char data_name[20];
	char last_name[20];
	int nn;
	double build_weight;
	double memory_weight;
	double target_precision;
	double sample_fraction;
	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:d:b:m:t:s:l:k:h",longopts, &ind);

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
      case 'l':
        if (optarg) {
			strcpy(last_name, optarg);
        }
        break;
      case 'k':
        if (optarg) {
			nn = atoi(optarg);
        }
        break;
      case 'b':
        if (optarg) {
          build_weight = atof(optarg);
        }
        break;
      case 'm':
        if (optarg) {
          memory_weight = atof(optarg);
        }
        break;
      case 't':
        if (optarg) {
          target_precision = atof(optarg);
        }
        break;
      case 's':
        if (optarg) {
          sample_fraction = atof(optarg);
        }
        break;
    }
	}

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

	char file_name[100];
	sprintf(file_name,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/data/dataset_%s.hdf5",file_path,last_name);
    load_from_file(dataset,file_name ,"dataset");
    load_from_file(query, file_name ,"query");
    load_from_file(groundtruth, file_name ,"groundtruth");

	ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/exp_fig/result/Auto/k=%d/%s_%.2fPrecision_%.2fBuild_%.0fMemory.txt",file_path,nn,last_name,target_precision,build_weight,memory_weight);
    out.open(result_path);
    
	char index_path[100];
	sprintf(index_path,"../../auto/%s/%.2fPrecision_%.2fBuild_%.0fMemory.index",data_name,target_precision,build_weight,memory_weight); 
	Index<L2<float> > index(dataset, flann::SavedIndexParams(index_path));                                                                      
  

    int checks[23]= {100,300,500,700,1000,2000,3000,4000,5000,7000,8000,10000,15000,20000,30000,40000,45000,50000,60000,70000,80000,90000,100000}; 
    for(int c=0;c<23;c++)
    {
    	Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    	Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    	double start_time_=clock();
    	index.knnSearch(query, indices, dists, nn, flann::SearchParams(checks[c]));
    	double search_time = double(clock()-start_time_)/CLOCKS_PER_SEC;

    	//compute the recall 
    	float recall = compute_recall(nn,indices,groundtruth);
		float rde = compute_relative_distance_error(nn,dataset,query,dists,groundtruth);
		float map = compute_mean_average_precision(nn,indices,groundtruth);

		out.setf(ios::fixed);
		out << recall << " " << rde << " " << search_time/query.rows <<" #N_"<< checks[c] <<" "<<map<<" "<<endl;
    	delete[] indices.ptr();
    	delete[] dists.ptr();
		if(recall==1)
			break;
	}

    delete[] dataset.ptr();
    delete[] query.ptr();
    
    return 0;
}


