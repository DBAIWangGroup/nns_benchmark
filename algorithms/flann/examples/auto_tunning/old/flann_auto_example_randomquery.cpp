
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
		int re_n=0;
		int gs_n=0;
		while(re_n<k && gs_n<k)
		{
			if(results[i][re_n] == gnds[i][gs_n])
			{
				count++;
				re_n++;
				gs_n++;
			}
			else
				gs_n++;
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

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char** argv)
{
	const struct option longopts[] ={
    {"help",                        no_argument,       0, 'h'},
    {"file_path",                   required_argument, 0, 'f'},
    {"data_name",                   required_argument, 0, 'd'},
    {"wb",                          required_argument, 0, 'b'},
    {"wm",                          required_argument, 0, 'm'},
    {"tp",                          required_argument, 0, 't'},
    {"sf",                          required_argument, 0, 's'},
    {"nn",                          required_argument, 0, 'k'},
  };
	int index;
  	int iarg = 0;
	char file_path[20];
	char data_name[10];
	int nn;
	double build_weight;
	double memory_weight;
	double target_precision;
	double sample_fraction;
	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:d:b:m:t:s:k:h",longopts, &index);

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
         // sample_fraction = atof(optarg);
        }
        break;
    }
	}

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

	char file_name[100];
	sprintf(file_name,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/data/dataset.hdf5",file_path);
    load_from_file(dataset,file_name ,"dataset");
    load_from_file(query, file_name ,"randomquery");
    load_from_file(groundtruth, file_name ,"groundtruth_randomquery");

	ofstream out;
	char result_path[100];
	sprintf(result_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/exp_fig/result/Auto/k=%d/flann_Auto_%.2fPrecision_%.2fBuild_%.0fMemory_randomquery.txt",file_path,nn,target_precision,build_weight,memory_weight);
    out.open(result_path);
    timeval start;
    timeval end;

	{
	sample_fraction = 1000.0/dataset.rows;
	char index_path[100];
	sprintf(index_path,"../../auto/%s/%.2fPrecision_%.2fBuild_%.0fMemory.index",data_name,target_precision,build_weight,memory_weight);
	Index<L2<float> > index(dataset, flann::SavedIndexParams(index_path)); 

	cout<<data_name << " k:"<<nn<<" target:"<<target_precision<<" build_weight:"<<build_weight<<"  memory_weight:"<<memory_weight<<" sample_fraction:" << sample_fraction<<" \n";
                                                                       
  
    int checks[16]= {50,100,300,500,700,1000,1500,2000,3000,4000,5000,7000,8000,10000,15000,20000}; //{100,200,300,500,700,1000,1500,2000,3000,5000,7000,10000,15000,20000,30000}; 
	float last_recall=0;
	float last_rde=0;
	float last_time=0;
    for(int c=0;c<16;c++)
    {
		if(last_recall==1)
		{
			out.setf(ios::fixed);
			out << last_recall << " " << last_rde << " " << last_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;
		}
		else
		{	
    		Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    		Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    		//start_time_=clock();
			gettimeofday(&start, NULL);
    		index.knnSearch(query, indices, dists, nn, flann::SearchParams(checks[c]));
			gettimeofday(&end, NULL);
    		double search_time = diff_timeval(end, start);

    		//compute the recall 
    		float recall = compute_recall(nn,indices,groundtruth);
			float rde = compute_relative_distance_error(nn,dataset,query,dists,groundtruth);
			last_recall = recall;
			last_rde = rde;
			last_time = search_time;
			out.setf(ios::fixed);
			out << recall << " " << rde << " " << search_time/query.rows <<" #N_"<< checks[c] <<" "<<endl;

    		delete[] indices.ptr();
    		delete[] dists.ptr();
		}
    	}
	}

    delete[] dataset.ptr();
    delete[] query.ptr();
    
    return 0;
}


