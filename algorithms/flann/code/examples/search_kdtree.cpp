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

float compute_recall(int k,const flann::Matrix<int>& results, const flann::Matrix<int>& gnds)
{
	int count = 0;
	for (unsigned i=0; i<results.rows; ++i) {
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
float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char** argv)
{

	const struct option longopts[] ={
    {"help",                        no_argument,       0, 'h'},
    {"file_path",                   required_argument, 0, 'f'},
    {"index_path",                  required_argument, 0, 'i'},
    {"result_path",                 required_argument, 0, 'r'},
    {"nn",                          required_argument, 0, 'k'},
    {"checks",                      required_argument, 0, 'c'},
  };
  	int ind;
  	int iarg = 0;
	char file_path[100];
	char index_path[100];
	char result_path[100];
	int nn;
	int c;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:i:r:k:c:h",longopts, &ind);

    switch (iarg) {
      case 'f':
        if (optarg) {
			strcpy(file_path, optarg);
        }
        break;
     case 'i':
        if (optarg) {
			strcpy(index_path, optarg);
        }
        break;
      case 'r':
        if (optarg) {
			strcpy(result_path, optarg);
        }
        break;
      case 'k':
        if (optarg) {
			nn = atoi(optarg);
        }
        break;
      case 'c':
        if (optarg) {
          c = atoi(optarg);
        }
        break;
    }
	}

    Matrix<float> dataset;
    Matrix<float> query;
    Matrix<int> groundtruth;

    load_from_file(dataset, file_path,"dataset");
    load_from_file(query, file_path,"query");
    load_from_file(groundtruth, file_path,"groundtruth");

    ofstream out;
    out.open(result_path,ios::app);
	Index<L2<float> > index(dataset, flann::SavedIndexParams(index_path)); 


    //do a knn search, using 128 checks
	Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

	timeval start;
    gettimeofday(&start, NULL);
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(c));
	timeval end;
    gettimeofday(&end, NULL);
    double search_time = diff_timeval(end, start);
   
    float recall = compute_recall(nn,indices,groundtruth);
	search_time=search_time/query.rows;
	
	out.setf(ios::fixed);
	out << recall <<" "<< search_time <<" #N_"<< c <<" "<<endl;

    delete[] indices.ptr();
    delete[] dists.ptr();
	delete[] dataset.ptr();
    delete[] query.ptr();
    
    return 0;
}


