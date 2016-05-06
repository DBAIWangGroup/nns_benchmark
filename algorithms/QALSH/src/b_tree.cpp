#include "headers.h"


// -----------------------------------------------------------------------------
//  BTree: b-tree to index hash values produced by qalsh
// -----------------------------------------------------------------------------
BTree::BTree()						// constructor
{
	root_ = -1;
	file_ = NULL;
	root_ptr_ = NULL;
}

// -----------------------------------------------------------------------------
BTree::~BTree()						// destructor
{
	if (root_ptr_ != NULL) {		// release <root_ptr_>
		delete root_ptr_; root_ptr_ = NULL;
	}

	char* header = new char[file_->get_blocklength()];
	write_header(header);			// write <root_> to <header>
	file_->set_header(header);		// write back to disk

	delete[] header; header = NULL;

	if (file_ != NULL) {			// release <file_>
		delete file_; file_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void BTree::init(					// init a new tree
	char* fname,						// file name
	int b_length)						// block length
{
	FILE* fp = fopen(fname, "r");
	if (fp) {						// check whether the file exist
		fclose(fp);					// ask whether replace?
		printf("The file \"%s\" exists. Replace? (y/n)", fname);
		
		char c ='y'; //getchar();			// input 'Y' or 'y' or others
		//getchar();					// input <ENTER>
		if (c != 'y' && c != 'Y') {	// if not remove existing file
			error("", true);		// program will be stopped.
		}
		remove(fname);			// otherwise, remove existing file
	}
									// init <file>, b-tree store here
	file_ = new BlockFile(fname, b_length);

	// -------------------------------------------------------------------------
	//  Init the first node: to store <blocklength> (page size of a node),
	//  <number> (number of nodes including both index node and leaf node), 
	//  and <root> (address of root node)
	// -------------------------------------------------------------------------
	root_ptr_ = new BIndexNode();
	root_ptr_->init(0, this);
	root_ = root_ptr_->get_block();
	delete_root();
}

// -----------------------------------------------------------------------------
void BTree::init_restore(			// load the tree from a tree file
	char* fname)						// file name
{
	FILE* fp = fopen(fname, "r");	// check whether the file exists
	if (!fp) {
		printf("tree file %s does not exist\n", fname);
		delete[] fname; fname = NULL;
		error("", true);
	}
	fclose(fp);

	// -------------------------------------------------------------------------
	//  It doesn't matter to initialize blocklength to 0.
	//  After reading file, <blocklength> will be reinitialized by file.
	// -------------------------------------------------------------------------
									// init <file>
	file_ = new BlockFile(fname, 0);
	root_ptr_ = NULL;				// init <root_ptr>

	// -------------------------------------------------------------------------
	//  Read the content after first 8 bytes of first block into <header>
	// -------------------------------------------------------------------------
	char* header = new char[file_->get_blocklength()];
	file_->read_header(header);		// read remain bytes from header
	read_header(header);			// init <root> from <header>

	if (header != NULL) {			// release space
		delete[] header; header = NULL;
	}
}

// -----------------------------------------------------------------------------
int BTree::read_header(				// read <root> from buffer
	char* buf)							// buffer
{
	memcpy(&root_, buf, SIZEINT);
	return SIZEINT;
}

// -----------------------------------------------------------------------------
int BTree::write_header(			// write <root> to buffer
	char* buf)							// buffer (return)
{
	memcpy(buf, &root_, SIZEINT);
	return SIZEINT;
}

// -----------------------------------------------------------------------------
void BTree::load_root()				// load <root_ptr> of b-tree
{
	if (root_ptr_ == NULL)  {
		root_ptr_ = new BIndexNode();
		root_ptr_->init_restore(this, root_);
	}
}

// -----------------------------------------------------------------------------
void BTree::delete_root()			// delete <root_ptr>
{
	if (root_ptr_ != NULL) {
		delete root_ptr_; root_ptr_ = NULL;
	}
}

// -----------------------------------------------------------------------------
int BTree::bulkload(				// bulkload a tree from memory
	HashValue* hashtable,				// hash table
	int n)								// number of entries
{
	BIndexNode* index_child   = NULL;
	BIndexNode* index_prev_nd = NULL;
	BIndexNode* index_act_nd  = NULL;

	BLeafNode* leaf_child   = NULL;
	BLeafNode* leaf_prev_nd = NULL;
	BLeafNode* leaf_act_nd  = NULL;

	int   id    = -1;
	int   block = -1;
	float key   = MINREAL;

	bool first_node  = false;		// determine relationship of sibling
	int  start_block = -1;			// position of first node
	int  end_block   = -1;			// position of last node

	int current_level    = -1;		// current level (leaf level is 0)
	int last_start_block = -1;		// to build b-tree level by level
	int last_end_block   = -1;		// to build b-tree level by level

	// -------------------------------------------------------------------------
	//  Build leaf node from <_hashtable> (level = 0)
	// -------------------------------------------------------------------------
	start_block = 0;
	end_block   = 0;
	first_node  = true;

	for (int i = 0; i < n; i++) {
		id = hashtable[i].id_;
		key = hashtable[i].proj_;

		if (!leaf_act_nd) {
			leaf_act_nd = new BLeafNode();
			leaf_act_nd->init(0, this);

			if (first_node) {
				first_node = false;	// init <start_block>
				start_block = leaf_act_nd->get_block();
			}
			else {					// label sibling
				leaf_act_nd->set_left_sibling(leaf_prev_nd->get_block());
				leaf_prev_nd->set_right_sibling(leaf_act_nd->get_block());

				delete leaf_prev_nd; leaf_prev_nd = NULL;
			}
			end_block = leaf_act_nd->get_block();
		}							// add new entry
		leaf_act_nd->add_new_child(id, key);	

		if (leaf_act_nd->isFull()) {// change next node to store entries
			leaf_prev_nd = leaf_act_nd;
			leaf_act_nd = NULL;
		}
	}
	if (leaf_prev_nd != NULL) {		// release the space
		delete leaf_prev_nd; leaf_prev_nd = NULL;
	}
	if (leaf_act_nd != NULL) {
		delete leaf_act_nd; leaf_act_nd = NULL;
	}

	// -------------------------------------------------------------------------
	//  Stop consition: lastEndBlock == lastStartBlock (only one node, as root)
	// -------------------------------------------------------------------------
	current_level = 1;				// build the b-tree level by level
	last_start_block = start_block;
	last_end_block = end_block;

	while (last_end_block > last_start_block) {
		first_node = true;
		for (int i = last_start_block; i <= last_end_block; i++) {
			block = i;				// get <block>
			if (current_level == 1) {
				leaf_child = new BLeafNode();
				leaf_child->init_restore(this, block);
				key = leaf_child->get_key_of_node();

				delete leaf_child; leaf_child = NULL;
			}
			else {
				index_child = new BIndexNode();
				index_child->init_restore(this, block);
				key = index_child->get_key_of_node();

				delete index_child; index_child = NULL;
			}

			if (!index_act_nd) {
				index_act_nd = new BIndexNode();
				index_act_nd->init(current_level, this);

				if (first_node) {
					first_node = false;
					start_block = index_act_nd->get_block();
				}
				else {
					index_act_nd->set_left_sibling(index_prev_nd->get_block());
					index_prev_nd->set_right_sibling(index_act_nd->get_block());

					delete index_prev_nd; index_prev_nd = NULL;
				}
				end_block = index_act_nd->get_block();
			}						// add new entry
			index_act_nd->add_new_child(key, block);

			if (index_act_nd->isFull()) {
				index_prev_nd = index_act_nd;
				index_act_nd = NULL;
			}
		}
		if (index_prev_nd != NULL) {// release the space
			delete index_prev_nd; index_prev_nd = NULL;
		}
		if (index_act_nd != NULL) {
			delete index_act_nd; index_act_nd = NULL;
		}
									// update info
		last_start_block = start_block;
		last_end_block = end_block;	// build b-tree of higher level
		current_level++;
	}
	root_ = last_start_block;		// update the <root>

	if (index_prev_nd != NULL) {
		delete index_prev_nd; index_prev_nd = NULL;
	}
	if (index_act_nd != NULL) {
		delete index_act_nd; index_act_nd = NULL;
	}
	if (index_child != NULL) {
		delete index_act_nd; index_act_nd = NULL;
	}
	if (leaf_prev_nd != NULL) {
		delete leaf_prev_nd; leaf_prev_nd = NULL;
	}
	if (leaf_act_nd != NULL) {
		delete leaf_act_nd; leaf_act_nd = NULL;
	}
	if (leaf_child != NULL) {
		delete leaf_child; leaf_child = NULL;
	}

	return 0;						// success to return
}
