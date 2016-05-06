#include "headers.h"


// -----------------------------------------------------------------------------
//  QALSH: structure of qalsh indexed by b+ tree. QALSH is used to solve the 
//  problem of approximate nearest neighbor search.
// -----------------------------------------------------------------------------
QALSH::QALSH()						// constructor
{
	n_pts_ = dim_ = B_ = -1;
	appr_ratio_ = beta_ = delta_ = -1.0;
	N_=100;

	w_ = p1_ = p2_ = alpha_ = -1.0;
	m_ = l_ = -1;

	a_array_ = NULL;
	trees_ = NULL;
}

// -----------------------------------------------------------------------------
QALSH::~QALSH()						// destructor
{
	if (a_array_) {
		delete[] a_array_; a_array_ = NULL;
		g_memory -= SIZEFLOAT * m_ * dim_;
	}

	if (trees_) {
		for (int i = 0; i < m_; i++) {
			delete trees_[i]; trees_[i] = NULL;
		}
		delete[] trees_; trees_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void QALSH::init(					// init params of qalsh
	int   n,							// number of points
	int   d,							// dimension of space
	int   B,							// page size
	float ratio, int N,						// approximation ratio
	char* output_folder)				// folder of info of qalsh
{
	n_pts_ = n;						// init <n_pts_>
	dim_   = d;						// init <dim_>
	B_     = B;						// init <B_>
	appr_ratio_ = ratio;			// init <appr_ratio_>
	N_ = N;

									// init <index_path_>
	strcpy(index_path_, output_folder);
	strcat(index_path_, "L2_indices/");

									// init <w_> <delta_> <beta_> <p1_>
	calc_params();					// <p2_> <alpha_> <m_> and <l_>

	gen_hash_func();				// init <a_array_>
	display_params();				// display params
	trees_ = NULL;					// init <trees_>
}

// -----------------------------------------------------------------------------
void QALSH::calc_params()			// calc params of qalsh
{
	// -------------------------------------------------------------------------
	//  init <delta_> and <beta_>
	// -------------------------------------------------------------------------
	delta_ = 1.0f / E;
	beta_  = N_*1.0 / n_pts_;   //modify 

	// -------------------------------------------------------------------------
	//  init <w_> <p1_> and <p2_>
	// -------------------------------------------------------------------------
									// <w> <p1> and <p2> of L2 distance
									// init <w> (auto tuning-w)
	w_ = sqrt((8.0f * appr_ratio_ * appr_ratio_ * log(appr_ratio_))
		/ (appr_ratio_ * appr_ratio_ - 1.0f));

	p1_ = calc_l2_prob(w_ / 2.0f);
	p2_ = calc_l2_prob(w_ / (2.0f * appr_ratio_));

	// -------------------------------------------------------------------------
	//  init <alpha_> <m_> and <l_>
	// -------------------------------------------------------------------------
	float para1 = sqrt(log(2.0f / beta_));
	float para2 = sqrt(log(1.0f / delta_));
	float para3 = 2.0f * (p1_ - p2_) * (p1_ - p2_);
	
	float eta = para1 / para2;		// init <alpha_>
	alpha_ = (eta * p1_ + p2_) / (1.0f + eta);
									// init <m_> and <l_>
	m_ = (int) ceil((para1 + para2) * (para1 + para2) / para3);
	l_ = (int) ceil((p1_ * para1 + p2_ * para2) * (para1 + para2) / para3);
}

// -----------------------------------------------------------------------------
float QALSH::calc_l2_prob(			// calc prob <p1_> and <p2_> of L2 dist
	float x)							// x = w / (2.0 * r)
{
	return new_gaussian_prob(x);
}

// -----------------------------------------------------------------------------
void QALSH::display_params()		// display params of qalsh
{

	printf("Parameters of QALSH (L_2 Distance):\n");

	printf("    n          = %d\n", n_pts_);
	printf("    d          = %d\n", dim_);
	printf("    B          = %d\n", B_);
	printf("    ratio      = %0.2f\n", appr_ratio_);
	printf("    w          = %0.4f\n", w_);
	printf("    p1         = %0.4f\n", p1_);
	printf("    p2         = %0.4f\n", p2_);
	printf("    alpha      = %0.6f\n", alpha_);
	printf("    beta       = %0.6f\n", beta_);
	printf("    delta      = %0.6f\n", delta_);
	printf("    m          = %d\n", m_);
	printf("    l          = %d\n", l_);
	printf("    beta * n   = %d\n", N_);
	printf("    index path = %s\n\n", index_path_);
}

// -----------------------------------------------------------------------------
void QALSH::gen_hash_func()			// generate hash function <a_array>
{
	int sum = m_ * dim_;

	g_memory += SIZEFLOAT * sum;
	a_array_ = new float[sum];

	for (int i = 0; i < sum; i++) {
		a_array_[i] = gaussian(0.0f, 1.0f);
	}
}

// -----------------------------------------------------------------------------
int QALSH::bulkload(				// build m b-trees by bulkloading
	float** data)						// data set
{
	// -------------------------------------------------------------------------
	//  Check whether the default maximum memory is enough
	// -------------------------------------------------------------------------
	g_memory += sizeof(HashValue) * n_pts_;
	if (check_mem()) {
		printf("*** memory = %.2f MB\n\n", g_memory / (1024.0f * 1024.0f));
		return 1;
	}

	// -------------------------------------------------------------------------
	//  Check whether the directory exists. If the directory does not exist, we
	//  create the directory for each folder.
	// -------------------------------------------------------------------------
									// create directory under Linux
	int len = (int) strlen(index_path_);
	for (int i = 0; i < len; i++) {
		if (index_path_[i] == '/') {
			char ch = index_path_[i + 1];
			index_path_[i + 1] = '\0';
									// check whether the directory exists
			int ret = access(index_path_, F_OK);
			if (ret != 0) {			// create directory
				ret = mkdir(index_path_, 0755);
				if (ret != 0) {
					printf("Could not create directory %s\n", index_path_);
					error("QALSH::bulkload error\n", true);
				}
			}
			index_path_[i + 1] = ch;
		}
	}
	
	/*								// create directory under Windows
	int len = (int) strlen(index_path_);
	for (int i = 0; i < len; i++) {
		if (index_path_[i] == '/') {
			char ch = index_path_[i + 1];
			index_path_[i + 1] = '\0';
									// check whether the directory exists
			int ret = _access(index_path_, 0);
			if (ret != 0) {			// create directory
				ret = _mkdir(index_path_);
				if (ret != 0) {
					printf("Could not create directory %s\n", index_path_);
					error("QALSH::bulkload() error\n", true);
				}
			}
			index_path_[i + 1] = ch;
		}
	}
	*/
	
	// -------------------------------------------------------------------------
	//  Write the file "para" where the parameters and hash functions are 
	//  stored in it.
	// -------------------------------------------------------------------------
	char fname[200];
	strcpy(fname, index_path_);		// write the "para" file
	strcat(fname, "para");
	if (write_para_file(fname)) return 1;

	// -------------------------------------------------------------------------
	//  Write the hash tables (indexed by b+ tree) to the disk
	// -------------------------------------------------------------------------
									// dataset sorted by hash value
	HashValue* hashtable = new HashValue[n_pts_];
	for (int i = 0; i < m_; i++) {
		printf("    Tree %3d (out of %d)\n", i + 1, m_);

		printf("        Computing Hash Values...\n");
		for (int j = 0; j < n_pts_; j++) {
			hashtable[j].id_ = j;
			hashtable[j].proj_ = calc_hash_value(i, data[j]);
		}

		printf("        Sorting...\n");
		qsort(hashtable, n_pts_, sizeof(HashValue), HashValueQsortComp);

		printf("        Bulkloading...\n");
		get_tree_filename(i, fname);

		BTree *bt = new BTree();
		bt->init(fname, B_);
		if (bt->bulkload(hashtable, n_pts_)) {
			return 1;
		}
		delete bt; bt = NULL;
	}

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (hashtable != NULL) {
		delete[] hashtable; hashtable = NULL;
		g_memory -= sizeof(HashValue) * n_pts_;
	}
	return 0;						// success to return
}

// -----------------------------------------------------------------------------
int QALSH::write_para_file(			// write "para" file from disk
	char* fname)						// file name of "para" file
{
	FILE* fp = NULL;
	fp = fopen(fname, "r");
	if (fp) {						// ensure the file not exist
		error("QALSH: hash tables exist.\n", true);
	}

	fp = fopen(fname, "w");			// open "para" file to write
	if (!fp) {
		printf("I could not create %s.\n", fname);
		printf("Perhaps no such folder %s?\n", index_path_);
		return 1;					// fail to return
	}

	fprintf(fp, "n = %d\n", n_pts_);// write <n_pts_>
	fprintf(fp, "d = %d\n", dim_);	// write <dim_>
	fprintf(fp, "B = %d\n", B_);	// write <B_>
									// write <appr_ratio_>
	fprintf(fp, "ratio = %f\n", appr_ratio_);
	fprintf(fp, "w = %f\n", w_);	// write <w_>
	fprintf(fp, "p1 = %f\n", p1_);	// write <p1_>
	fprintf(fp, "p2 = %f\n", p2_);	// write <p2_>
									// write <alpha_>
	fprintf(fp, "alpha = %f\n", alpha_);
									// write <beta_>
	fprintf(fp, "beta = %f\n", beta_);
									// write <delta_>
	fprintf(fp, "delta = %f\n", delta_);

	fprintf(fp, "m = %d\n", m_);	// write <m_>
	fprintf(fp, "l = %d\n", l_);	// write <l_>

	int count = 0;
	for (int i = 0; i < m_; i++) {	// write <a_array_>
		fprintf(fp, "%f", a_array_[count++]);
		for (int j = 1; j < dim_; j++) {
			fprintf(fp, " %f", a_array_[count++]);
		}
		fprintf(fp, "\n");
	}
	if (fp) fclose(fp);				// close para file
	
	return 0;						// success to return
}

// -----------------------------------------------------------------------------
float QALSH::calc_hash_value(		// calc hash value
	int table_id,						// hash table id
	float* point)						// a point
{
	float ret = 0.0f;
	for (int i = 0; i < dim_; i++) {
		ret += (a_array_[table_id * dim_ + i] * point[i]);
	}
	return ret;
}

// -----------------------------------------------------------------------------
void QALSH::get_tree_filename(		// get file name of b-tree
	int tree_id,						// tree id, from 0 to m-1
	char* fname)						// file name (return)
{
	char c[20];

	strcpy(fname, index_path_);
	sprintf(c, "%d", tree_id);
	strcat(fname, c);
	strcat(fname, ".qalsh");
}

// -----------------------------------------------------------------------------
int QALSH::restore(					// load existing b-trees.
	char* output_folder)				// folder of info of qalsh
{
									// init <index_path_>
	strcpy(index_path_, output_folder);
	strcat(index_path_, "L2_indices/");

	char fname[200];
	strcpy(fname, index_path_);
	strcat(fname, "para");

	if (read_para_file(fname)) {	// read "para" file and init params
		return 1;					// fail to return
	}

	trees_ = new BTree*[m_];		// allocate <trees>
	for (int i = 0; i < m_; i++) {
		get_tree_filename(i, fname);// get filename of tree

		trees_[i] = new BTree();	// init <trees>
		trees_[i]->init_restore(fname);
	}
	return 0;						// success to return
}

// -----------------------------------------------------------------------------
int QALSH::read_para_file(			// read "para" file
	char* fname)						// file name of "para" file
{
	FILE* fp = NULL;
	fp = fopen(fname, "r");
	if (!fp) {						// ensure we can open the file
		printf("QALSH::read_para_file could not open %s.\n", fname);
		return 1;
	}

	fscanf(fp, "n = %d\n", &n_pts_);// read <n_pts_>
	fscanf(fp, "d = %d\n", &dim_);	// read <dim_>
	fscanf(fp, "B = %d\n", &B_);	// read <B_>
									// read <appr_ratio_>
	fscanf(fp, "ratio = %f\n", &appr_ratio_);
	fscanf(fp, "w = %f\n", &w_);	// read <w_>
	fscanf(fp, "p1 = %f\n", &p1_);	// read <p1_>
	fscanf(fp, "p2 = %f\n", &p2_);	// read <p2_>
									// read <alpha_>
	fscanf(fp, "alpha = %f\n", &alpha_);
									// read <beta_>
	fscanf(fp, "beta = %f\n", &beta_);
									// read <delta_>
	fscanf(fp, "delta = %f\n", &delta_);

	fscanf(fp, "m = %d\n", &m_);	// read <m_>
	fscanf(fp, "l = %d\n", &l_);	// read <l_>

	a_array_ = new float[m_ * dim_];// read <a_array_>
	g_memory += SIZEFLOAT * m_ * dim_;
	int count = 0;
	for (int i = 0; i < m_; i++) {
		for (int j = 0; j < dim_; j++) {
			fscanf(fp, "%f", &a_array_[count++]);
		}
		fscanf(fp, "\n");
	}
	if (fp) fclose(fp);				// close para file
	
	display_params();				// display params
	return 0;						// success to return
}

// -----------------------------------------------------------------------------
int QALSH::knn(						// k-nn search
	float* query,						// query point
	int top_k,							// top-k value
	ResultItem* rslt,					// k-nn results
	char* output_folder)				// output folder
{
	// -------------------------------------------------------------------------
	//  Space allocation and initialization
	// -------------------------------------------------------------------------
									// init k-nn results
	for (int i = 0; i < top_k; i++) {
		rslt[i].id_   = -1;
		rslt[i].dist_ = MAXREAL;
	}
									// objects frequency
	int* frequency  = new int[n_pts_];
	for (int i = 0; i < n_pts_; i++) {
		frequency[i]  = 0;
	}
									// whether an object is checked
	bool* is_checked = new bool[n_pts_];
	for (int i = 0; i < n_pts_; i++) {
		is_checked[i] = false;
	}

	float* data = new float[dim_];	// one object data
	for (int i = 0; i < dim_; i++) {
		data[i] = 0.0f;
	}
	g_memory += ((SIZEBOOL + SIZEINT) * n_pts_ + SIZEFLOAT * dim_);

	bool* flag = new bool[m_];		// whether a hash table is finished
	for (int i = 0; i < m_; i++) {
		flag[i] = true;
	}

	float* q_val = new float[m_];	// hash value of query
	for (int i = 0; i < m_; i++) {
		q_val[i] = -1.0f;
	}
	g_memory += (SIZEFLOAT + SIZEBOOL) * m_;
									// left and right page buffer
	PageBuffer* lptr = new PageBuffer[m_];
	PageBuffer* rptr = new PageBuffer[m_];
	g_memory += (SIZECHAR * B_ * m_ * 2 + SIZEINT * m_ * 6);

	for (int i = 0; i < m_; i++) {
		lptr[i].leaf_node_ = NULL;
		lptr[i].index_pos_ = -1;
		lptr[i].leaf_pos_  = -1;
		lptr[i].size_      = -1;

		rptr[i].leaf_node_ = NULL;
		rptr[i].index_pos_ = -1;
		rptr[i].leaf_pos_  = -1;
		rptr[i].size_      = -1;
	}

	// -------------------------------------------------------------------------
	//  Compute hash value <q_dist> of query and init the page buffers 
	//  <lptr> and <rptr>.
	// -------------------------------------------------------------------------
	page_io_ = 0;					// num of page i/os
	dist_io_ = 0;					// num of dist cmpt
	init_buffer(lptr, rptr, q_val, query);

	// -------------------------------------------------------------------------
	//  Determine the basic <radius> and <bucket_width> 
	// -------------------------------------------------------------------------
	float radius = find_radius(lptr, rptr, q_val);
	float bucket_width = (w_ * radius / 2.0f);

	// -------------------------------------------------------------------------
	//  K-nn search
	// -------------------------------------------------------------------------
	bool again      = true;			// stop flag
	int  candidates = 99 + top_k;	// threshold of candidates
	int  flag_num   = 0;			// used for bucket bound
	int  scanned_id = 0;			// num of scanned id
	int checked=0;

	int id    = -1;					// current object id
	int count = -1;					// count size in one page
	int start = -1;					// start position
	int end   = -1;					// end position

	float left_dist = -1.0f;		// left dist with query
	float right_dist = -1.0f;		// right dist with query
	float knn_dist = MAXREAL;		// kth nn dist
									// result entry for update
	ResultItem* item = new ResultItem();
	g_memory += (long) sizeof(ResultItem);

	while (again) {
		// ---------------------------------------------------------------------
		//  Step 1: initialize the stop condition for current round
		// ---------------------------------------------------------------------
		flag_num = 0;
		for (int i = 0; i < m_; i++) {
			flag[i] = true;
		}

		// ---------------------------------------------------------------------
		//  Step 2: find frequent objects
		// ---------------------------------------------------------------------
		while (true) {
			for (int i = 0; i < m_; i++) {
				if (!flag[i]) continue;

				// -------------------------------------------------------------
				//  Step 2.1: compute <left_dist> and <right_dist>
				// -------------------------------------------------------------
				left_dist = -1.0f;
				if (lptr[i].size_ != -1) {
					left_dist = calc_proj_dist(&lptr[i], q_val[i]);
				}

				right_dist = -1.0f;
				if (rptr[i].size_ != -1) {
					right_dist = calc_proj_dist(&rptr[i], q_val[i]);
				}

				// -------------------------------------------------------------
				//  Step 2.2: determine the closer direction (left or right)
				//  and do collision counting to find frequent objects.
				//
				//  For the frequent object, we calc the L2 distance with
				//  query, and update the k-nn result.
				// -------------------------------------------------------------
				if (left_dist >= 0 && left_dist < bucket_width && 
					((right_dist >= 0 && left_dist <= right_dist) ||
					right_dist < 0)) {

					count = lptr[i].size_;
					end = lptr[i].leaf_pos_;
					start = end - count;
					for (int j = end; j > start; j--) {
						id = lptr[i].leaf_node_->get_entry_id(j);
						frequency[id]++;
						scanned_id++;

						if (frequency[id] > l_ && !is_checked[id]) {
							is_checked[id] = true;
							read_data(id, dim_, B_, data, output_folder);

							item->dist_ = calc_l2_dist(data, query, dim_);
							item->id_ = id;
							knn_dist = update_result(rslt, item, top_k);
							checked++;
							// -------------------------------------------------
							//  Terminating condition 2
							// -------------------------------------------------
							dist_io_++;
							if (dist_io_ >= candidates) {
								again = false;
								flag_num += m_;
								break;
							}
						}
					}
					update_left_buffer(&lptr[i], &rptr[i]);
				}
				else if (right_dist >= 0 && right_dist < bucket_width && 
					((left_dist >= 0 && left_dist > right_dist) || 
					left_dist < 0)) {

					count = rptr[i].size_;
					start = rptr[i].leaf_pos_;
					end = start + count;
					for (int j = start; j < end; j++) {
						id = rptr[i].leaf_node_->get_entry_id(j);
						frequency[id]++;
						scanned_id++;
						if (frequency[id] > l_ && !is_checked[id]) {
							is_checked[id] = true;
							read_data(id, dim_, B_, data, output_folder);

							item->dist_ = calc_l2_dist(data, query, dim_);
							item->id_ = id;
							knn_dist = update_result(rslt, item, top_k);
   							checked++;
							// -------------------------------------------------
							//  Terminating condition 2
							// -------------------------------------------------
							dist_io_++;
							if (dist_io_ >= candidates) {
								again = false;
								flag_num += m_;
								break;
							}
						}
					}
					update_right_buffer(&lptr[i], &rptr[i]);
				}
				else {
					flag[i] = false;
					flag_num++;
				}
				if (flag_num >= m_) break;
			}
			if (flag_num >= m_) break;
		}
		// ---------------------------------------------------------------------
		//  Terminating condition 1
		// ---------------------------------------------------------------------
		if (knn_dist < appr_ratio_ * radius && dist_io_ >= top_k) {
			again = false;
			break;
		}

		// ---------------------------------------------------------------------
		//  Step 3: auto-update <radius>
		// ---------------------------------------------------------------------
		radius = update_radius(lptr, rptr, q_val, radius);
		bucket_width = radius * w_ / 2.0f;
	}

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (data != NULL || frequency != NULL || is_checked != NULL) {
		delete[] data; data = NULL;
		delete[] frequency;  frequency  = NULL;
		delete[] is_checked; is_checked = NULL;
		g_memory -= ((SIZEBOOL + SIZEINT) * n_pts_ + SIZEFLOAT * dim_);
	}
	if (q_val != NULL || flag != NULL || item != NULL) {
		delete[] q_val; q_val = NULL;
		delete[] flag; flag = NULL;
		delete   item; item = NULL;
		g_memory -= (SIZEFLOAT + SIZEBOOL) * m_;
		g_memory -= (long) sizeof(ResultItem);
	}

	for (int i = 0; i < m_; i++) {
		// ---------------------------------------------------------------------
		//  CANNOT remove the condition
		//              <lptrs[i].leaf_node != rptrs[i].leaf_node>
		//  Because <lptrs[i].leaf_node> and <rptrs[i].leaf_node> may point 
		//  to the same address, then we would delete it twice and receive 
		//  the runtime error or segmentation fault.
		// ---------------------------------------------------------------------
		if (lptr[i].leaf_node_ && lptr[i].leaf_node_ != rptr[i].leaf_node_) {
			delete lptr[i].leaf_node_; lptr[i].leaf_node_ = NULL;
		}
		if (rptr[i].leaf_node_) {
			delete rptr[i].leaf_node_; rptr[i].leaf_node_ = NULL;
		}
	}
	delete[] lptr; lptr = NULL;
	delete[] rptr; rptr = NULL;
	g_memory -= (SIZECHAR * B_ * m_ * 2 + SIZEINT * m_ * 6);
	return (page_io_ + dist_io_);
}

// -----------------------------------------------------------------------------
void QALSH::init_buffer(			// init page buffer (loc pos of b-treee)
	PageBuffer* lptr,					// left buffer page (return)
	PageBuffer* rptr,					// right buffer page (return)
	float* q_dist,						// hash value of query (return)
	float* query)						// query point
{
	int  block   = -1;				// tmp vars for index node
	int  follow  = -1;
	bool lescape = false;

	int pos = -1;					// tmp vars for leaf node
	int increment = -1;
	int num_entries = -1;

	BIndexNode* index_node = NULL;

	for (int i = 0; i < m_; i++) {	// calc hash value of query
		q_dist[i] = calc_hash_value(i, query);
		block = trees_[i]->root_;

		index_node = new BIndexNode();
		index_node->init_restore(trees_[i], block);
		page_io_++;

		// ---------------------------------------------------------------------
		//  Find the leaf node whose value is closest and larger than the key
		//  of query q <qe->key>
		// ---------------------------------------------------------------------
		lescape = false;			// locate the position of branch
		while (index_node->get_level() > 1) {
			follow = index_node->find_position_by_key(q_dist[i]);

			if (follow == -1) {		// if in the most left branch
				if (lescape) {		// scan the most left branch
					follow = 0;
				}
				else {
					if (block != trees_[i]->root_) {
						error("QALSH::knn_bucket No branch found\n", true);
					}
					else {
						follow = 0;
						lescape = true;
					}
				}
			}
			block = index_node->get_son(follow);
			delete index_node; index_node = NULL;

			index_node = new BIndexNode();
			index_node->init_restore(trees_[i], block);
			page_io_++;				// access a new node (a new page)
		}

		// ---------------------------------------------------------------------
		//  After finding the leaf node whose value is closest to the key of
		//  query, initialize <lptrs[i]> and <rptrs[i]>.
		//
		//  <lescape> = true is that the query has no <lptrs>, the query is 
		//  the smallest value.
		// ---------------------------------------------------------------------
		follow = index_node->find_position_by_key(q_dist[i]);
		if (follow < 0) {
			lescape = true;
			follow = 0;
		}

		if (lescape) {				// only init right buffer
			block = index_node->get_son(0);
			rptr[i].leaf_node_ = new BLeafNode();
			rptr[i].leaf_node_->init_restore(trees_[i], block);
			rptr[i].index_pos_ = 0;
			rptr[i].leaf_pos_ = 0;

			increment = rptr[i].leaf_node_->get_increment();
			num_entries = rptr[i].leaf_node_->get_num_entries();
			if (increment > num_entries) {
				rptr[i].size_ = num_entries;
			} else {
				rptr[i].size_ = increment;
			}
			page_io_++;
		}
		else {						// init left buffer
			block = index_node->get_son(follow);
			lptr[i].leaf_node_ = new BLeafNode();
			lptr[i].leaf_node_->init_restore(trees_[i], block);

			pos = lptr[i].leaf_node_->find_position_by_key(q_dist[i]);
			if (pos < 0) pos = 0;
			lptr[i].index_pos_ = pos;

			increment = lptr[i].leaf_node_->get_increment();
			if (pos == lptr[i].leaf_node_->get_num_keys() - 1) {
				num_entries = lptr[i].leaf_node_->get_num_entries();
				lptr[i].leaf_pos_ = num_entries - 1;
				lptr[i].size_ = num_entries - pos * increment;
			}
			else {
				lptr[i].leaf_pos_ = pos * increment + increment - 1;
				lptr[i].size_ = increment;
			}
			page_io_++;
									// init right buffer
			if (pos < lptr[i].leaf_node_->get_num_keys() - 1) {
				rptr[i].leaf_node_ = lptr[i].leaf_node_;
				rptr[i].index_pos_ = (pos + 1);
				rptr[i].leaf_pos_ = (pos + 1) * increment;
				
				if ((pos + 1) == rptr[i].leaf_node_->get_num_keys() - 1) {
					num_entries = rptr[i].leaf_node_->get_num_entries();
					rptr[i].size_ = num_entries - (pos + 1) * increment;
				}
				else {
					rptr[i].size_ = increment;
				}
			}
			else {
				rptr[i].leaf_node_ = lptr[i].leaf_node_->get_right_sibling();
				if (rptr[i].leaf_node_) {
					rptr[i].index_pos_ = 0;
					rptr[i].leaf_pos_ = 0;

					increment = rptr[i].leaf_node_->get_increment();
					num_entries = rptr[i].leaf_node_->get_num_entries();
					if (increment > num_entries) {
						rptr[i].size_ = num_entries;
					} else {
						rptr[i].size_ = increment;
					}
					page_io_++;
				}
			}
		}

		if (index_node != NULL) {
			delete index_node; index_node = NULL;
		}
	}
}

// -----------------------------------------------------------------------------
float QALSH::find_radius(			// find proper radius
	PageBuffer* lptr,					// left page buffer
	PageBuffer* rptr,					// right page buffer
	float* q_dist)						// hash value of query
{
	float radius = update_radius(lptr, rptr, q_dist, 1.0f/appr_ratio_);
	if (radius < 1.0f) radius = 1.0f;

	return radius;
}

// -----------------------------------------------------------------------------
float QALSH::update_radius(			// update radius
	PageBuffer* lptr,					// left page buffer
	PageBuffer* rptr,					// right page buffer
	float* q_dist,						// hash value of query
	float  old_radius)					// old radius
{
	float dist = 0.0f;				// tmp vars
	vector<float> list;

	for (int i = 0; i < m_; i++) {	// find an array of proj dist
		if (lptr[i].size_ != -1) {
			dist = calc_proj_dist(&lptr[i], q_dist[i]);
			list.push_back(dist);		
		}
		if (rptr[i].size_ != -1) {
			dist = calc_proj_dist(&rptr[i], q_dist[i]);
			list.push_back(dist);
		}
	}
	sort(list.begin(), list.end());	// sort the array

	int num = (int) list.size();
	if (num % 2 == 0) {				// find median dist
		dist = (list[num/2 - 1] + list[num/2]) / 2.0f;
	} else {
		dist = list[num/2];
	}
	list.clear();

	if (compfloats(dist, old_radius) == 0) {
		int kappa = (int) ceil(log(2.0f * dist / w_) / log(appr_ratio_));
		dist = pow(appr_ratio_, kappa);
	}
	return dist;
}

// -----------------------------------------------------------------------------
float QALSH::update_result(			// update knn results
	ResultItem* rslt,					// k-nn results
	ResultItem* item,					// new result
	int top_k)							// top-k value
{
	int i = -1;
	int pos = -1;
	bool alreadyIn = false;

	for (i = 0; i < top_k; i++) {
									// ensure the id is not exist before
		if (item->id_ == rslt[i].id_) {
			alreadyIn = true;
			break;
		}							// find the position to insert
		else if (compfloats(item->dist_, rslt[i].dist_) == -1) {
			break;
		}
	}
	pos = i;

	if (!alreadyIn && pos < top_k) {// insertion
		for (i = top_k - 1; i > pos; i--) {
			rslt[i].setto(&(rslt[i - 1]));
		}
		rslt[pos].setto(item);
	}
	return rslt[top_k - 1].dist_;
}

// -----------------------------------------------------------------------------
void QALSH::update_left_buffer(		// update left buffer
	PageBuffer* lptr,					// left buffer
	const PageBuffer* rptr)				// right buffer
{
	BLeafNode* leaf_node = NULL;
	BLeafNode* old_leaf_node = NULL;

	if (lptr->index_pos_ > 0) {
		lptr->index_pos_--;

		int pos = lptr->index_pos_;
		int increment = lptr->leaf_node_->get_increment();
		lptr->leaf_pos_ = pos * increment + increment - 1;
		lptr->size_ = increment;
	}
	else {
		old_leaf_node = lptr->leaf_node_;
		leaf_node = lptr->leaf_node_->get_left_sibling();

		if (leaf_node) {
			lptr->leaf_node_ = leaf_node;
			lptr->index_pos_ = lptr->leaf_node_->get_num_keys() - 1;

			int pos = lptr->index_pos_;
			int increment = lptr->leaf_node_->get_increment();
			int num_entries = lptr->leaf_node_->get_num_entries();
			lptr->leaf_pos_ = num_entries - 1;
			lptr->size_ = num_entries - pos * increment;
			page_io_++;
		}
		else {
			lptr->leaf_node_ = NULL;
			lptr->index_pos_ = -1;
			lptr->leaf_pos_ = -1;
			lptr->size_ = -1;
		}

		if (rptr->leaf_node_ != old_leaf_node) {
			delete old_leaf_node; old_leaf_node = NULL;
		}
	}
}

// -----------------------------------------------------------------------------
void QALSH::update_right_buffer(	// update right buffer
	const PageBuffer* lptr,				// left buffer
	PageBuffer* rptr)					// right buffer
{
	BLeafNode* leaf_node = NULL;
	BLeafNode* old_leaf_node = NULL;

	if (rptr->index_pos_ < rptr->leaf_node_->get_num_keys() - 1) {
		rptr->index_pos_++;

		int pos = rptr->index_pos_;
		int increment = rptr->leaf_node_->get_increment();
		rptr->leaf_pos_ = pos * increment;
		if (pos == rptr->leaf_node_->get_num_keys() - 1) {
			int num_entries = rptr->leaf_node_->get_num_entries();
			rptr->size_ = num_entries - pos * increment;
		}
		else {
			rptr->size_ = increment;
		}
	}
	else {
		old_leaf_node = rptr->leaf_node_;
		leaf_node = rptr->leaf_node_->get_right_sibling();

		if (leaf_node) {
			rptr->leaf_node_ = leaf_node;
			rptr->index_pos_ = 0;
			rptr->leaf_pos_ = 0;

			int increment = rptr->leaf_node_->get_increment();
			int num_entries = rptr->leaf_node_->get_num_entries();
			if (increment > num_entries) {
				rptr->size_ = num_entries;
			} else {
				rptr->size_ = increment;
			}
			page_io_++;
		}
		else {
			rptr->leaf_node_ = NULL;
			rptr->index_pos_ = -1;
			rptr->leaf_pos_ = -1;
			rptr->size_ = -1;
		}

		if (lptr->leaf_node_ != old_leaf_node) {
			delete old_leaf_node; old_leaf_node = NULL;
		}
	}
}

// -----------------------------------------------------------------------------
float QALSH::calc_proj_dist(		// calc proj dist
	const PageBuffer* ptr,				// page buffer
	float q_val)						// hash value of query
{
	int pos = ptr->index_pos_;
	float key = ptr->leaf_node_->get_key(pos);
	float dist = fabs(key - q_val);

	return dist;
}

// -----------------------------------------------------------------------------
//  Comparison function for qsort called by QALSH::bulkload()
// -----------------------------------------------------------------------------
int HashValueQsortComp(				// compare function for qsort
	const void* e1,						// 1st element
	const void* e2)						// 2nd element
{
	int ret = 0;
	HashValue* value1 = (HashValue *) e1;
	HashValue* value2 = (HashValue *) e2;

	if (value1->proj_ < value2->proj_) {
		ret = -1;
	} else if (value1->proj_ > value2->proj_) {
		ret = 1;
	} else {
		if (value1->id_ < value2->id_) ret = -1;
		else if (value1->id_ > value2->id_) ret = 1;
	}
	return ret;
}
