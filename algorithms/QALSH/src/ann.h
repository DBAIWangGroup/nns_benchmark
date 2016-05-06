#ifndef __ANN_H
#define __ANN_H

// -----------------------------------------------------------------------------
int ground_truth(					// output ground truth (data in memory)
	int   n,							// number of data points
	int   qn,							// number of query points
	int   d,							// dimension of space
	char* data_set,						// address of data set
	char* query_set,					// address of query set
	char* truth_set);					// address of ground truth file

// -----------------------------------------------------------------------------
int indexing(						// build hash tables for the dataset
	int   n,							// number of data points
	int   d,							// dimension of space
	int   B,							// page size
	float ratio, int N,						// approximation ratio
	char* data_set,						// address of data set
	char* output_folder,char* index_folder);				// folder to store info of qalsh

// -----------------------------------------------------------------------------
int lshknn(							// k-nn via qalsh (data in disk)
	int   qn,							// number of query points
	int   d, int N,						// dimensionality
	char* query_set,					// path of query set
	char* truth_set,					// groundtrue file
	char* output_folder,char* result_folder,float ratio,int B);				// output folder

// -----------------------------------------------------------------------------
int linear_scan(					// brute-force linear scan (data in disk)
	int   n,							// number of data points
	int   qn,							// number of query points
	int   d,							// dimension of space
	int   B,							// page size
	char* query_set,					// address of query set
	char* truth_set,					// address of ground truth file
	char* output_folder);				// output folder


#endif
