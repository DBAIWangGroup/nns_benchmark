#ifndef __B_TREE_H
#define __B_TREE_H


class BlockFile;
class BNode;

struct HashValue;

// -----------------------------------------------------------------------------
//  BTree: b-tree to index hash tables produced by qalsh
// -----------------------------------------------------------------------------
class BTree {
public:
	int root_;						// address of disk for root

	BlockFile* file_;				// file in disk to store
	BNode* root_ptr_;				// pointer of root

	// -------------------------------------------------------------------------
	BTree();						// constructor
	~BTree();						// destructor

	// -------------------------------------------------------------------------
	void init(						// init a new b-tree
		char* fname,					// file name
		int b_length);					// block length

	void init_restore(				// load an exist b-tree
		char* fname);					// file name

	// -------------------------------------------------------------------------
	int bulkload(					// bulkload b-tree from hash table in mem
		HashValue* hashtable,			// hash table
		int n);							// number of entries

private:
	// -------------------------------------------------------------------------
	int read_header(				// read <root> from buffer
		char* buf);						// the buffer

	int write_header(				// write <root> into buffer
		char* buf);						// the buffer (return)

	// -------------------------------------------------------------------------
	void load_root();				// load root of b-tree

	void delete_root();				// delete root of b-tree
};

#endif
