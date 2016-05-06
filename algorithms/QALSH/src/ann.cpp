#include "headers.h"


float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

//-----------------------------------------------------------------------------

int ground_truth(					// output the ground truth results
	int   n,							// number of data points
	int   qn,							// number of query points
	int   d,							// dimension of space
	char* data_set,						// address of data set
	char* query_set,					// address of query set
	char* truth_set)					// address of ground truth file
{
	clock_t startTime = (clock_t) -1;
	clock_t endTime   = (clock_t) -1;

	int i, j;
	FILE* fp = NULL;

	// -------------------------------------------------------------------------
	//  Read data set and query set
	// -------------------------------------------------------------------------
	startTime = clock();
	g_memory += SIZEFLOAT * (n + qn) * d;
	if (check_mem()) return 1;

	float** data = new float*[n];
	for (i = 0; i < n; i++) data[i] = new float[d];
	if (read_set(n, d, data_set, data)) {
		error("Reading Dataset Error!\n", true);
	}

	float** query = new float*[qn];
	for (i = 0; i < qn; i++) query[i] = new float[d];
	if (read_set(qn, d, query_set, query) == 1) {
		error("Reading Query Set Error!\n", true);
	}
	endTime = clock();
	printf("Read Dataset and Query Set: %.6f Seconds\n\n", 
		((float) endTime - startTime) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  output ground truth results (using linear scan method)
	// -------------------------------------------------------------------------
	int maxk = MAXK;
	float dist = -1.0F;
	float* knndist = new float[maxk];
	g_memory += SIZEFLOAT * maxk;

	fp = fopen(truth_set, "w");		// open output file
	if (!fp) {
		printf("I could not create %s.\n", truth_set);
		return 1;
	}

	fprintf(fp, "%d %d\n", qn, maxk);
	for (i = 0; i < qn; i++) {
		for (j = 0; j < maxk; j++) {
			knndist[j] = MAXREAL;
		}
									// find k-nn points of query
		for (j = 0; j < n; j++) {
			dist = calc_l2_dist(data[j], query[i], d);

			int ii, jj;
			for (jj = 0; jj < maxk; jj++) {
				if (compfloats(dist, knndist[jj]) == -1) {
					break;
				}
			}
			if (jj < maxk) {
				for (ii = maxk - 1; ii >= jj + 1; ii--) {
					knndist[ii] = knndist[ii - 1];
				}
				knndist[jj] = dist;
			}
		}

		fprintf(fp, "%d", i + 1);	// output Lp dist of k-nn points
		for (j = 0; j < maxk; j++) {
			fprintf(fp, " %f", knndist[j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);						// close output file
	endTime = clock();
	printf("Generate Ground Truth: %.6f Seconds\n\n", 
		((float) endTime - startTime) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (data != NULL) {				// release <data>
		for (i = 0; i < n; i++) {
			delete[] data[i]; data[i] = NULL;
		}
		delete[] data; data = NULL;
		g_memory -= SIZEFLOAT * n * d;
	}
	if (query != NULL) {			// release <query>
		for (i = 0; i < qn; i++) {
			delete[] query[i]; query[i] = NULL;
		}
		delete[] query; query = NULL;
		g_memory -= SIZEFLOAT * qn * d;
	}
	if (knndist != NULL) {			// release <knndist>
		delete[] knndist; knndist = NULL;
		g_memory -= SIZEFLOAT * maxk;
	}

	//printf("memory = %.2f MB\n", (float) g_memory / (1024.0f * 1024.0f));
	return 0;
}

// -----------------------------------------------------------------------------
int indexing(						// build hash tables for the dataset
	int   n,							// number of data points
	int   d,							// dimension of space
	int   B,							// page size
	float ratio, int N,						// approximation ratio
	char* data_set,						// address of data set
	char* output_folder,char* index_folder)				// folder to store info of qalsh
{
	//c_show = ratio;
	clock_t startTime = (clock_t) -1;
	clock_t endTime   = (clock_t) -1;

	// -------------------------------------------------------------------------
	//  Read data set
	// -------------------------------------------------------------------------
	startTime = clock();
	g_memory += SIZEFLOAT * n * d;	
	if (check_mem()) return 1;

	float** data = new float*[n];
	for (int i = 0; i < n; i++) data[i] = new float[d];
	if (read_set(n, d, data_set, data) == 1) {
		error("Reading Dataset Error!\n", true);
	}
	endTime = clock();
	printf("Read Dataset: %.6f Seconds\n\n", 
		((float) endTime - startTime) / CLOCKS_PER_SEC);

	char fname[200];
	strcpy(fname, index_folder);
	//strcat(fname, "L2_index.out");

	FILE* fp = fopen(fname, "a+");
	if (!fp) {
		printf("I could not create %s.\n", fname);
		return 1;					// fail to return
	}

	// -------------------------------------------------------------------------
	//  Write the data set in new format to disk
	// -------------------------------------------------------------------------
		
	timeval start;
	gettimeofday(&start, NULL);	
	write_data_new_form(n, d, B, data, output_folder);
		
	QALSH* lsh = new QALSH();
	lsh->init(n, d, B, ratio,N, output_folder);
	lsh->bulkload(data);
	timeval end;
	gettimeofday(&end, NULL);
	float index_time = diff_timeval(end,start);

	fprintf(fp, "%.6f #c_%.2f #B_%d #index_time \n", index_time, ratio, B);
	fclose(fp);

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (data != NULL) {
		for (int i = 0; i < n; i++) {
			delete[] data[i]; data[i] = NULL;
		}
		delete[] data; data = NULL;
		g_memory -= SIZEFLOAT * n * d;
	}
	if (lsh != NULL) {
		delete lsh; lsh = NULL;
	}

	return 0;
}

// -----------------------------------------------------------------------------
int lshknn(							// k-nn via qalsh (data in disk)
	int   qn,							// number of query points
	int   d, int N,							// dimensionality
	char* query_set,					// path of query set
	char* truth_set,					// groundtrue file
	char* output_folder,char* result_folder,float ratio,int B)				// output folder
{
	int ret = 0;
	int maxk = MAXK;
	int i, j;
	FILE* fp = NULL;				// file pointer

	// -------------------------------------------------------------------------
	//  Read query set
	// -------------------------------------------------------------------------
	g_memory += SIZEFLOAT * qn * d;
	float** query = new float*[qn];
	for (i = 0; i < qn; i++) query[i] = new float[d];
	if (read_set(qn, d, query_set, query)) {
		error("Reading Query Set Error!\n", true);
	}
	// -------------------------------------------------------------------------
	//  Read the ground truth file
	// -------------------------------------------------------------------------
	g_memory += SIZEFLOAT * qn * maxk;
	//float* R = new float[qn * maxk];
	int* R = new int[qn * maxk];

	fp = fopen(truth_set, "r");		// open ground truth file
	if (!fp) {
		printf("Could not open the ground truth file.\n");
		return 1;
	}//*
	for (int i = 0; i < qn; i++) {
		for (j = 0; j < maxk; j ++) {
			fscanf(fp, "%d ", &(R[i * maxk + j]));
		}
	}
	fclose(fp);	

	int kNNs[] = {20};
	int maxRound = 1;
	int top_k = 0;

	float allRatio  = -1.0f;
	float thisRatio = -1.0f;
	int allIO=0;
									// init the results
	g_memory += (long) sizeof(ResultItem) * maxk;
	ResultItem* rslt = new ResultItem[maxk];
	for (i = 0; i < maxk; i++) {
		rslt[i].id_ = -1;
		rslt[i].dist_ = MAXREAL;
	}

	QALSH* lsh = new QALSH();		// restore QALSH
	if (lsh->restore(output_folder)) {
		error("Could not restore qalsh\n", true);
	}

	char output_set[200];
	strcpy(output_set, result_folder);
	//strcat(output_set, "20K.txt");

	fp = fopen(output_set, "a+");	//open output file
	if (!fp) {
		printf("Could not create the output file.\n");
		return 1;
	}

	printf("QALSH for c-k-ANN Search: \n");
	printf("  Top-k\tRatio\t\tI/O\t\tTime (ms)\n");
	for (int num = 0; num < maxRound; num++) {
		top_k = kNNs[num];
		allRatio = 0.0f;
		float search_time=0;
		for (i = 0; i < qn; i++) { //qn
			timeval start;
			gettimeofday(&start, NULL);
			int thisIO =lsh->knn(query[i], top_k, rslt, output_folder);
			timeval end;
			gettimeofday(&end, NULL);
			search_time += diff_timeval(end,start);
			allIO += thisIO;
			float found=0;
			int re_n=0;
			for(int j=0;j<top_k;j++)
			{
				if(rslt[j].id_==R[i * maxk + j])
				{
					found++;
					re_n++;	
				}
			}
			allRatio += found;
		}

		/*
		for (i = 0; i < qn*3; i++) { //qn
			timeval start;
			gettimeofday(&start, NULL);
			lsh->knn(query[i/3], top_k, rslt, output_folder);
			timeval end;
			gettimeofday(&end, NULL);
			if(i%3==2)
			{
				search_time += diff_timeval(end,start);
				float found=0;
				int re_n=0;
				for(int j=0;j<top_k;j++)
				{
					if(rslt[j].id_==R[i/3 * maxk + j])
					{
						found++;
						re_n++;	
					}
				}
				allRatio += found;
			}
		}*/
		allRatio = allRatio / qn/maxk;
		//fprintf(fp, " \n\n ");
		fprintf(fp, "%.6f %d %.6f \n", allRatio, allIO/qn, search_time/qn);
	}
	printf("\n");
	fclose(fp);						// close output file

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (query != NULL) {			// release <query>
		for (i = 0; i < qn; i++) {
			delete[] query[i]; query[i] = NULL;
		}
		delete[] query; query = NULL;
		g_memory -= SIZEFLOAT * qn * d;
	}
	if (lsh != NULL) {				// release <lsh>
		delete lsh; lsh = NULL;
	}
									// release <R> and (/or) <rslt>
	if (R != NULL || rslt != NULL) {
		delete[] R; R = NULL;
		delete[] rslt; rslt = NULL;
		g_memory -= (SIZEFLOAT * qn * maxk + sizeof(ResultItem) * maxk);
	}

	//printf("memory = %.2f MB\n", (float) g_memory / (1024.0f * 1024.0f));
	return ret;
}

// -----------------------------------------------------------------------------
int linear_scan(					// brute-force linear scan (data in disk)
	int   n,							// number of data points
	int   qn,							// number of query points
	int   d,							// dimension of space
	int   B,							// page size
	char* query_set,					// address of query set
	char* truth_set,					// address of ground truth file
	char* output_folder)				// output folder
{
	// -------------------------------------------------------------------------
	//  Allocation and initialzation.
	// -------------------------------------------------------------------------
	clock_t startTime = (clock_t) -1.0f;
	clock_t endTime   = (clock_t) -1.0f;

	int kNNs[] = {1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	int maxRound = 11;
	
	int i, j, top_k;
	int maxk = MAXK;

	float allTime   = -1.0f;
	float thisRatio = -1.0f;
	float allRatio  = -1.0f;

	g_memory += (SIZEFLOAT * (d + d + (qn + 1) * maxk) + SIZECHAR * (600 + B));
	
	float* knn_dist = new float[maxk];
	for (i = 0; i < maxk; i++) {
		knn_dist[i] = MAXREAL;
	}

	float** R = new float*[qn];
	for (i = 0; i < qn; i++) {
		R[i] = new float[maxk];
		for (j = 0; j < maxk; j++) {
			R[i][j] = 0.0f;
		}
	}

	float* data     = new float[d];	// one data object
	float* query    = new float[d];	// one query object

	char* buffer    = new char[B];	// every time can read one page
	char* fname     = new char[200];// file name for data
	char* data_path = new char[200];// data path
	char* out_set	= new char[200];// output file

	// -------------------------------------------------------------------------
	//  Open the output file, and read the ground true results
	// -------------------------------------------------------------------------
	strcpy(out_set, output_folder);	// generate output file
	strcat(out_set, "L2_linear.out");

	FILE* ofp = fopen(out_set, "w");
	if (!ofp) {
		printf("I could not create %s.\n", out_set);
		return 1;
	}
									// open ground true file
	FILE* tfp = fopen(truth_set, "r");
	if (!tfp) {
		printf("I could not create %s.\n", truth_set);
		return 1;
	}
									// read top-k nearest distance
	fscanf(tfp, "%d %d\n", &qn, &maxk);
	for (int i = 0; i < qn; i++) {
		fscanf(tfp, "%d", &j);
		for (j = 0; j < maxk; j ++) {
			fscanf(tfp, " %f", &(R[i][j]));
		}
	}
	fclose(tfp);					// close ground true file

	// -------------------------------------------------------------------------
	//  Calc the number of data object in one page and the number of data file.
	//  <num> is the number of data in one data file
	//  <total_file> is the total number of data file
	// -------------------------------------------------------------------------
	int num = (int) floor((float) B / (d * SIZEFLOAT));
	int total_file = (int) ceil((float) n / num);
	if (total_file == 0) return 1;

	// -------------------------------------------------------------------------
	//  Brute-force linear scan method (data in disk)
	//  For each query, we limit that we can ONLY read one page of data.
	// -------------------------------------------------------------------------
	int count = 0;
	float dist = -1.0F;
									// generate the data path
	strcpy(data_path, output_folder);
	strcat(data_path, "data/");

	printf("Linear Scan Search:\n");
	printf("    Top-k\tRatio\t\tI/O\t\tTime (ms)\n");
	for (int round = 0; round < maxRound; round++) {
		top_k = kNNs[round];
		allRatio = 0.0f;

		startTime = clock();
		FILE* qfp = fopen(query_set, "r");
		if (!qfp) error("Could not open the query set.\n", true);

		for (i = 0; i < qn; i++) {
			// -----------------------------------------------------------------
			//  Step 1: read a query from disk and init the k-nn results
			// -----------------------------------------------------------------
			fscanf(qfp, "%d", &j);
			for (j = 0; j < d; j++) {
				fscanf(qfp, " %f", &query[j]);
			}

			for (j = 0; j < top_k; j++) {
				knn_dist[j] = MAXREAL;
			}

			// -----------------------------------------------------------------
			//  Step 2: find k-nn results for the query
			// -----------------------------------------------------------------
			for (j = 0; j < total_file; j++) {
				// -------------------------------------------------------------
				//  Step 2.1: get the file name of current data page
				// -------------------------------------------------------------
				get_data_filename(j, data_path, fname);

				// -------------------------------------------------------------
				//  Step 2.2: read one page of data into buffer
				// -------------------------------------------------------------
				if (read_buffer_from_page(B, fname, buffer) == 1) {
					error("error to read a data page", true);
				}

				// -------------------------------------------------------------
				//  Step 2.3: find the k-nn results in this page. NOTE: the 
				// 	number of data in the last page may be less than <num>
				// -------------------------------------------------------------
				if (j < total_file - 1) count = num;
				else count = n % num;

				for (int z = 0; z < count; z++) {
					read_data_from_buffer(z, d, data, buffer);
					dist = calc_l2_dist(data, query, d);

					int ii, jj;
					for (jj = 0; jj < top_k; jj++) {
						if (compfloats(dist, knn_dist[jj]) == -1) {
							break;
						}
					}
					if (jj < top_k) {
						for (ii = top_k - 1; ii >= jj + 1; ii--) {
							knn_dist[ii] = knn_dist[ii - 1];
						}
						knn_dist[jj] = dist;
					}
				}
			}

			thisRatio = 0.0f;
			for (j = 0; j < top_k; j++) {
				thisRatio += knn_dist[j] / R[i][j];
			}
			thisRatio /= top_k;
			allRatio += thisRatio;
		}
		// -----------------------------------------------------------------
		//  Step 3: output result of top-k nn points
		// -----------------------------------------------------------------
		fclose(qfp);				// close query file
		endTime  = clock();
		allTime  = ((float) endTime - startTime) / 1000.0f;
		allTime  = allTime  / qn;
		allRatio = allRatio / qn;
									// output results
		printf("    %3d\t\t%.4f\t\t%d\t\t%.2f\n", top_k, allRatio, 
			total_file, allTime);
		fprintf(ofp, "%d\t%f\t%d\t%f\n", top_k, allRatio, total_file, allTime);
	}
	printf("\n");
	fclose(ofp);						// close output file

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (R != NULL) {
		for (i = 0; i < qn; i++) {
			delete[] R[i]; R[i] = NULL;
		}
		delete[] R; R = NULL;
	}
	if (knn_dist != NULL || buffer != NULL || data != NULL || query != NULL) {
		delete[] knn_dist; knn_dist = NULL;
		delete[] buffer; buffer = NULL;
		delete[] data; data = NULL;
		delete[] query; query = NULL;
	}
	if (fname != NULL || data_path != NULL || out_set != NULL) {
		delete[] fname; fname = NULL;
		delete[] data_path; data_path = NULL;
		delete[] out_set; out_set = NULL;
	}
	g_memory -= (SIZEFLOAT * (d + d + (qn + 1) * maxk) + SIZECHAR * (600 + B));
	
	//printf("memory = %.2f MB\n", (float) g_memory / (1024.0f * 1024.0f));
	return 0;
}
