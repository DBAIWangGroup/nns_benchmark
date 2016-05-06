#ifndef __QALSH_H
#define __QALSH_H

class BNode;
class BTree;

// -----------------------------------------------------------------------------
//  ResultItem: structure of result item which is used to k-nn search
// -----------------------------------------------------------------------------
struct ResultItem {
	int   id_;						// id of the point
	float dist_;					// l2 distance to query

	void setto(ResultItem * item) {
		id_   = item->id_;
		dist_ = item->dist_;
	}
};

// -----------------------------------------------------------------------------
//  PageBuffer: buffer of a page for the ANN search of qalsh
// -----------------------------------------------------------------------------
struct PageBuffer {
	BLeafNode* leaf_node_;			// leaf node (level = 0)
	int index_pos_;					// cur pos of index key
	int leaf_pos_;					// cur pos of leaf node
	int size_;						// size for one scan
};

// -----------------------------------------------------------------------------
//  HashValue: structure of hash table to store hash values
// -----------------------------------------------------------------------------
struct HashValue {
	int id_;						// object id
	float proj_;					// projection of the object
};

// -----------------------------------------------------------------------------
//  QALSH: structure of qalsh indexed by b+ tree. QALSH is used to solve
//  the problem of Approximate Nearest Neighbor (ANN) search.
// -----------------------------------------------------------------------------
class QALSH {
public:
	QALSH();						// constructor
	~QALSH();						// destructor

	// -------------------------------------------------------------------------
	void init(						// init params of qalsh
		int   n,						// number of data points
		int   d,						// dimension of space
		int   B,						// page size
		float ratio, int N,					// approximation ratio
		char* output_folder);			// folder of info of qalsh

	// -------------------------------------------------------------------------
	int restore(					// restore params of qalsh
		char* output_folder);			// folder of info of qalsh

	// -------------------------------------------------------------------------
	int bulkload(					// build b+ trees by bulkloading
		float** data);					// data set

	// -------------------------------------------------------------------------
	int knn(						// k-nn search
		float* query,					// one query point
		int top_k,						// top-k value
		ResultItem* rslt,				// k-nn results
		char* output_folder);			// folder of info of qalsh

private:
	// -------------------------------------------------------------------------
	int   n_pts_;					// number of points
	int   dim_;						// dimensiona of space
	int   B_;						// page size in words
	float appr_ratio_;				// approximation ratio

	// -------------------------------------------------------------------------
	float w_;						// bucket width
	float p1_;						// positive probability
	float p2_;						// negative probability

	float alpha_;					// collision threshold percentage
	float beta_;					// false positive percentage
	float delta_;					// error probability
	int N_;

	int m_;							// number of hashtables
	int l_;							// collision threshold

	float* a_array_;				// hash functions
	char index_path_[200];			// folder path of index

	int dist_io_;					// io for calculating L2 distance
	int page_io_;					// io for scanning pages by qalsh
	BTree** trees_;					// b-trees

	// -------------------------------------------------------------------------
	void calc_params();				// calc parama of qalsh

	float calc_l2_prob(				// calc <p1> and <p2> for L2 distance
		float x);						// para

	void display_params();			// display params

	void gen_hash_func();			// generate hash functions

	// -------------------------------------------------------------------------
	float calc_hash_value(			// calc hash value
		int table_id,					// hash table id
		float* point);					// one point

	int write_para_file(			// write file of para
		char* fname);					// file name of para

	int read_para_file(				// read file of para
		char* fname);					// file name of para

	void get_tree_filename(			// get file name of tree
		int tree_id,					// tree id
		char* fname);					// file name of tree (return)

	// -------------------------------------------------------------------------
	void init_buffer(				// init page buffer (loc pos of b-treee)
		PageBuffer* lptr,				// left buffer page (return)
		PageBuffer* rptr,				// right buffer page (return)
		float* q_dist,					// hash value of query (return)
		float* query);					// query point

	// -------------------------------------------------------------------------
	float find_radius(				// find proper radius
		PageBuffer* lptr,				// left page buffer
		PageBuffer* rptr,				// right page buffer
		float* q_dist);					// hash value of query

	// -------------------------------------------------------------------------
	float update_radius(			// update radius
		PageBuffer* lptr,				// left page buffer
		PageBuffer* rptr,				// right page buffer
		float* q_dist,					// hash value of query
		float  old_radius);				// old radius

	// -------------------------------------------------------------------------
	float update_result(			// update k-nn results
		ResultItem* rslt,				// k-nn results
		ResultItem* item,				// input result
		int topk);						// top-k value

	// -------------------------------------------------------------------------
	void update_left_buffer(		// update left buffer
		PageBuffer* lptr,				// left buffer
		const PageBuffer* rptr);		// right buffer

	void update_right_buffer(		// update right buffer
		const PageBuffer* lptr,			// left buffer
		PageBuffer* rptr);				// right buffer

	// -------------------------------------------------------------------------
	float calc_proj_dist(			// calc proj dist
		const PageBuffer* ptr,			// page buffer
		float q_val);					// hash value of query
};


// -----------------------------------------------------------------------------
//  Comparison function for qsort called by QALSH::bulkload()
// -----------------------------------------------------------------------------
int HashValueQsortComp(				// compare function for qsort
	const void* e1,						// 1st element
	const void* e2);					// 2nd element

#endif
