
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
	return sqrt(dist);
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
	{"pre_name",                    required_argument, 0, 'p'},
	{"suf_name",                    required_argument, 0, 's'},
  };
  	int ind;
  	int iarg = 0;
	char file_path[20];
	char data_name[10];
	char pre_name[10];
	char suf_name[10];

	while (iarg != -1) {
    	iarg = getopt_long(argc, argv, "f:d:p:s:h",longopts, &ind);

    	switch (iarg) 
		{
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
    	}
	}

    Matrix<float> dataset;
    Matrix<float> query;
	char data_path[100];
	sprintf(data_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/data/%sdataset%s.hdf5",file_path,pre_name,suf_name);
    load_from_file(dataset, data_path ,"dataset");
    load_from_file(query, data_path ,"query");

    //build index
    Index<L2<float> > index(dataset, flann::LinearIndexParams());
    index.buildIndex();
                                                                                          
    int nn=100;
    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);
    index.knnSearch(query, indices, dists, nn, flann::SearchParams());
	cout<<indices[0][0]<<dists[0][0]<<endl;
	int n=dataset.rows;
	int nq=query.rows;
	int dim=query.cols;
	char result_path[100];
	sprintf(result_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/exp_fig/data/AQ/quality.txt",file_path);
	ofstream out;
	out.open(result_path,ios::app);

	int ks[5]={0,9,19,49,99};
	for(int i=0;i<5;i++)
	{
		vector<vector<int> >  stst;
		for(int j=0;j<nq;j++)
		{
			vector<int> s(10,0);
			float knn_dist=dists[j][ks[i]];
			for(int t=0;t<n;t++)
			{
				float dist=get_distance(query[j],dataset[t],dim);
				for(int r=0;r<10;r++)
				{
					if(dist<=(r+1)*knn_dist)
					{
						for(int g=r;g<10;g++)
							s[g]++;
						break;
					}
				}
			}
			stst.push_back(s);
		}
		for(int j=0;j<10;j++)
		{
			int count=0;
			for(int t=0;t<nq;t++)
			{
				if(stst[t][j]>=n/2)
					count++;
			}
			out<<count*1.0/nq<<" ";
		}
		out<<"\n";
		cout<<"finish:"<<ks[i]<<endl;
	}
	cout<<"finish all"<<endl;
    delete[] indices.ptr();
    delete[] dists.ptr();
    
    delete[] dataset.ptr();
    delete[] query.ptr();
    return 0;
}


