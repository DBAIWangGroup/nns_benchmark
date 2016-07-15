#include "util.h"
#include <getopt.h>

using namespace std;

int main(int argc, char** argv)
{
	const struct option longopts[] ={
    {"help",                       no_argument,       0, 'h'},
    {"filename",                   required_argument, 0, 'f'},
    {"name",                       required_argument, 0, 'n'},
    {"output_path",                required_argument, 0, 'o'},
	{"file_type",                  required_argument, 0, 't'},
    {"data_type",                  required_argument, 0, 'y'},
  };

	int index;
    int iarg = 0;
    opterr = 1; 

	char filename[100]="";
	char name[100]="";
	char output_path[100] = ""; 
	int  file_type =2 ;
	bool is_integer = true;
	bool is_valid_command = true;

	while (iarg != -1) {
    iarg = getopt_long(argc, argv, "f:n:o:t:y:h",
                       longopts, &index);

    switch (iarg) {
		case 'f':
        	if (optarg) {
        		strcpy(filename, optarg);
          	}
        break;
		case 'n':
        	if (optarg) {
        		strcpy(name, optarg);
          	}
        break;
		case 'o':
        	if (optarg) {
        		strcpy(output_path, optarg);
          	}
        break;
		case 't':
          	if (optarg) {
            	if (strcmp(optarg, "txt") == 0) {
              		file_type = 0;
            	}
				else if (strcmp(optarg, "fvecs") == 0 || strcmp(optarg, "ivecs") == 0) {
					file_type = 1;
				} 
				else if (strcmp(optarg, "lshkit") != 0) {
              		is_valid_command = false;
            	}
          	}
        break;
		case 'y':
          	if (optarg) {
            	if (strcmp(optarg, "f") == 0) {
              		is_integer = false;
            	} else if (strcmp(optarg, "i") != 0) {
              		is_valid_command = false;
            	}
          	}
        break;
        }
      }
	
	if(!is_valid_command)
	{
		cerr<<"you must assign the type of the data \n";
		return 0;
	}
	int n;
	int dim;
	if(!is_integer)
	{
		cerr <<"float";
		float* dataset = load_from_file<float>(filename, name, &n, &dim);
		if(file_type == 0)
			save_to_txt<float>(output_path, dataset, n, dim);
		else if(file_type ==1)
			save_to_fvecs<float>(output_path, dataset, n, dim);
		else
			save_to_lshkit<float>(output_path, dataset, n, dim);
	}
	else
	{
		int* dataset = load_from_file<int>(filename, name, &n, &dim);
		if(file_type == 0)
			save_to_txt<int>(output_path, dataset, n, dim);
		else if(file_type ==1)
			save_to_fvecs<int>(output_path, dataset, n, dim);
		else
			save_to_lshkit<int>(output_path, dataset, n, dim);
	}


	//cout<<n<<" "<<dim<<endl;
	//float* query = load_from_file(filename,"query", &nq, &dim);

	//int k=20;
	//int* gnd = get_gnd(dataset,query,n,nq,dim,k);

	//cout<<gnd[0]<<" "<<gnd[1]<<" "<<gnd[20]<<endl;
	
	//save_to_txt<int>("audio.txt",gnd, nq, k);
	//save_to_fvecs("audio.fvecs",dataset, n, dim);
	//save_to_lshkit("audio.lshkit",dataset, n, dim);

	//float* dataset = read_from_txt("audio.txt",200,192);
	//float* dataset = read_from_fvecs("audio.fvecs", &n, &dim);
	//float* dataset = (float *)read_from_lshkit("audio.lshkit", &n, &dim);
	//save_to_file(dataset,n,dim,"audio.hdf5","query");
	//cout<<dataset[0]<<" "<<dataset[192]<<endl;
	return 0;
}
