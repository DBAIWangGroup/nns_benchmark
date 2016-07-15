#include "util.h"
#include "gnd.h"
#include <getopt.h>

using namespace std;

int main(int argc, char** argv)
{
	const struct option longopts[] ={
    {"help",                       no_argument,       0, 'h'},
    {"filename",                   required_argument, 0, 'f'},
    {"k",                          required_argument, 0, 'k'},
  };

	int index;
    int iarg = 0;
    opterr = 1; 

	char filename[100]="";
	int k =20;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:k:h",
                       longopts, &index);

    switch (iarg) {
		case 'f':
        	if (optarg) {
        		strcpy(filename, optarg);
          	}
        break;
		case 'k':
        	if (optarg) {
        		k = atoi(optarg);
          	}
        break;
        }
    }
	
	int n;
	int nq;
	int dim;
	float* dataset = load_from_file<float>(filename,"dataset",&n,&dim);
	float* query = load_from_file<float>(filename,"query", &nq, &dim);
	int* gnd = get_gnd(dataset,query,n,nq,dim,k);
	save_to_file<int>(gnd,nq,k,filename,"groundtruth");
	
	return 0;
}
