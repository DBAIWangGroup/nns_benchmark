#include <hdf5.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <typeinfo>


using namespace std;

template<typename T>
hid_t get_hdf5_type()
{
    //throw FLANNException("Unsupported type for IO operations");
}

template<>
hid_t get_hdf5_type<char>() { return H5T_NATIVE_CHAR; }
template<>
hid_t get_hdf5_type<unsigned char>() { return H5T_NATIVE_UCHAR; }
template<>
hid_t get_hdf5_type<short int>() { return H5T_NATIVE_SHORT; }
template<>
hid_t get_hdf5_type<unsigned short int>() { return H5T_NATIVE_USHORT; }
template<>
hid_t get_hdf5_type<int>() { return H5T_NATIVE_INT; }
template<>
hid_t get_hdf5_type<unsigned int>() { return H5T_NATIVE_UINT; }
template<>
hid_t get_hdf5_type<long>() { return H5T_NATIVE_LONG; }
template<>
hid_t get_hdf5_type<unsigned long>() { return H5T_NATIVE_ULONG; }
template<>
hid_t get_hdf5_type<float>() { return H5T_NATIVE_FLOAT; }
template<>
hid_t get_hdf5_type<double>() { return H5T_NATIVE_DOUBLE; }
template<>
hid_t get_hdf5_type<long double>() { return H5T_NATIVE_LDOUBLE; }

template<typename T>
T* load_from_file(const char* filename, const char* name, int* n, int* d)
{
    herr_t status;
    hid_t file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
	hid_t dataset_id = H5Dopen2(file_id, name, H5P_DEFAULT);
    hid_t space_id = H5Dget_space(dataset_id);

    hsize_t dims_out[2];
    H5Sget_simple_extent_dims(space_id, dims_out, NULL);
	*n = (int)dims_out[0];
	*d = (int)dims_out[1];
    T* dataset=new T[dims_out[0] * dims_out[1]];
    status = H5Dread(dataset_id, get_hdf5_type<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, dataset);
    //H5T_NATIVE_FLOAT
    H5Sclose(space_id);
    H5Dclose(dataset_id);
    H5Fclose(file_id);

	return dataset;
}

template<typename T>
void save_to_file(const T* dataset,int n, int d, const char* filename, const char* name)
{
    H5Eset_auto( H5E_DEFAULT, NULL, NULL );
    herr_t status;
    hid_t file_id;
    file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
    if (file_id < 0) {
        file_id = H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
    }

    hsize_t dimsf[2];              // dataset dimensions
    dimsf[0] = n;
    dimsf[1] = d;

    hid_t space_id = H5Screate_simple(2, dimsf, NULL);
    hid_t memspace_id = H5Screate_simple(2, dimsf, NULL);

    hid_t dataset_id = H5Dcreate2(file_id, name, get_hdf5_type<T>(), space_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    if (dataset_id<0) {
        dataset_id = H5Dopen2(file_id, name, H5P_DEFAULT);
    }

    status = H5Dwrite(dataset_id, get_hdf5_type<T>(), memspace_id, space_id, H5P_DEFAULT, dataset);

    H5Sclose(memspace_id);
    H5Sclose(space_id);
    H5Dclose(dataset_id);
    H5Fclose(file_id);
}
//*
template<typename T>
T* read_from_fvecs(const char* filename, int* len, int* d)
{
	FILE * dfp = fopen(filename, "rb");
	int dim;
	fread(&dim,sizeof(int),1,dfp);
	fseek(dfp, 0, SEEK_END);
	off_t size = ftell(dfp);
    fseek(dfp, 0, SEEK_SET);
    int n = (int)(size / (sizeof(int)+ sizeof(T)*dim));
	*d = dim;
	*len = n;
	T* data = new T[n * dim];
	for(int i=0 ; i<n ; i++)
  	{
		fread(&dim, sizeof(int), 1, dfp);
		fread(&data[i*dim], sizeof(T), dim, dfp);
  	}
	fclose(dfp);
	return data;
}

template<typename T>
T* read_from_txt(const char* filename, int n, int d)
{
	FILE * dfp = fopen(filename, "r");
	T * data = new float [n * d];
	for(int i=0 ; i<n ; i++)
  	{
		if(typeid(T)==typeid(int))
			for(int j=0;j<d;j++)
				fscanf(dfp,"%d ",&data[i*d + j]);	
		if(typeid(T)==typeid(float))
			for(int j=0;j<d;j++)
				fscanf(dfp,"%f ",&data[i*d + j]);
  	}
	fclose(dfp);
	return data;
}

static const unsigned LSHKIT_HEADER = 3;
template<typename T>
char* read_from_lshkit(const char* filename, int* len, int* dim)
{
	ifstream is(filename, ios::binary);
    unsigned header[LSHKIT_HEADER]; // entry size, row, col 
    is.read((char *)header, sizeof header);
    
    int d = header[2];
    unsigned skip = LSHKIT_HEADER * sizeof(unsigned);   

    is.seekg(0, std::ios::end);
	size_t size = is.tellg();
    size -= skip;
    unsigned line = sizeof(T) * d;
    int n =  size/line; 
    *len=n;
	*dim=d;
    is.seekg(skip, std::ios::beg);
    char* data = new char[n*sizeof(T)*d];

    for (unsigned i = 0; i < n; ++i) {
        is.read( data + i*sizeof(T)*d , sizeof(T) * d);      
    }    
    is.close();
    return data;

}

template<typename T>
void save_to_fvecs(const char* filename, const T* dataset, int n, int d)
{
	FILE * ofp = fopen(filename, "w");
	for(int i=0;i<n;i++)
	{
		fwrite(&d,sizeof(int),1,ofp);
		fwrite(&dataset[i*d],sizeof(T),d,ofp);
	}
	fclose(ofp);	

}

template<typename T>
void save_to_txt(const char* filename, T* dataset, int n, int d)
{
	FILE * ofp = fopen(filename, "w");
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<d;j++)
		{
			if(typeid(T)==typeid(int))
				fprintf(ofp,"%d ",dataset[i*d+j]);
			if(typeid(T)==typeid(float))
				fprintf(ofp,"%f ",dataset[i*d+j]);
		}
		if(i<n-1)
			fprintf(ofp,"\n");
	}
	fclose(ofp);
}

template<typename T>
void save_to_lshkit(const char* filename, const T* dataset, int n, int d)
{
    FILE* os = fopen(filename, "wb");
	
    int tmp = 4;
	fwrite(&tmp, sizeof(tmp),1,os);
	fwrite(&n, sizeof(n),1,os);
	fwrite(&d, sizeof(d),1,os);

    for (int i = 0; i < n; ++i) {
        fwrite(&dataset[i*d], sizeof(T), d, os);
    }
    fclose(os);
}

