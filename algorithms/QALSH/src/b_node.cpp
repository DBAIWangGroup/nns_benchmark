#include "headers.h"


// -----------------------------------------------------------------------------
//  BNode: basic structure of node in b-tree
// -----------------------------------------------------------------------------
BNode::BNode()						// constructor
{
	level_ = -1;
	num_entries_ = -1;
	left_sibling_ = right_sibling_ = -1;
	key_ = NULL;

	block_ = capacity_ = -1;
	dirty_ = false;
	btree_ = NULL;
}

// -----------------------------------------------------------------------------
BNode::~BNode()						// destructor
{
	key_ = NULL;
	btree_ = NULL;
}

// -----------------------------------------------------------------------------
void BNode::init(					// init a new node, which not exist
	int level,							// level (depth) in b-tree
	BTree* btree)						// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = (char) level;			// init <level_>

	dirty_ = true;					// init <dirty_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	key_ = NULL;					// init <key_>

	num_entries_ = 0;				// init <num_entries_>
	block_ = -1;						// init <block_>
	capacity_ = -1;					// init <capacity_>
}

// -----------------------------------------------------------------------------
void BNode::init_restore(			// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>

	dirty_ = false;					// init <dirty_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	key_ = NULL;					// init <key_>

	num_entries_ = 0;				// init <num_entries_>
	level_ = -1;						// init <block_>
	capacity_ = -1;					// init <capacity_>
}

// -----------------------------------------------------------------------------
int BNode::get_entry_size()			// get entry size of b-node
{
	return 0;						// return nothing
}

// -----------------------------------------------------------------------------
void BNode::read_from_buffer(		// do nothing
	char* buf)
{
}

// -----------------------------------------------------------------------------
void BNode::write_to_buffer(		// do nothing
	char* buf)
{
}

// -----------------------------------------------------------------------------
int BNode::find_position_by_key(	// find pos just less than input key
	float key)							// input key
{
	return -1;						// do nothing
}

// -----------------------------------------------------------------------------
float BNode::get_key(				// get <key> indexed by <index>
	int index)							// input <index>
{
	return -1.0f;					// do nothing
}

// -----------------------------------------------------------------------------
BNode* BNode::get_left_sibling()	// get the left-sibling node
{
	BNode* node = NULL;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BNode();			// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
BNode* BNode::get_right_sibling()	// get the right-sibling node
{
	BNode* node = NULL;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BNode();			// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BNode::get_block()				// get <block_> (address of this node)
{
	return block_;
}

// -----------------------------------------------------------------------------
int BNode::get_num_entries()		// get <num_entries_>
{
	return num_entries_;
}

// -----------------------------------------------------------------------------
int BNode::get_level()				// get <level_>
{
	return level_;
}

// -----------------------------------------------------------------------------
//	<level>: SIZECHAR
//	<num_entries> <left_sibling> and <right_sibling>: SIZEINT
// -----------------------------------------------------------------------------
int BNode::get_header_size()		// get header size of b-node
{
	int header_size = SIZECHAR + SIZEINT * 3;
	return header_size;
}

// -----------------------------------------------------------------------------
float BNode::get_key_of_node()		// get key of this node
{
	return key_[0];
}

// -----------------------------------------------------------------------------
bool BNode::isFull()				// whether is full?
{
	if (num_entries_ >= capacity_) return true;
	else return false;
}

// -----------------------------------------------------------------------------
void BNode::set_left_sibling(		// set addr of left sibling node
	int left_sibling)					// addr of left sibling node
{
	left_sibling_ = left_sibling;
}

// -----------------------------------------------------------------------------
void BNode::set_right_sibling(		// set addr of right sibling node
	int right_sibling)					// addr of right sibling node
{
	right_sibling_ = right_sibling;
}



// -----------------------------------------------------------------------------
//  BIndexNode: structure of index node for b-tree
// -----------------------------------------------------------------------------
BIndexNode::BIndexNode()			// constructor
{
	level_ = -1;
	num_entries_ = -1;
	left_sibling_ = right_sibling_ = -1;

	block_ = capacity_ = -1;
	dirty_ = false;
	btree_ = NULL;

	key_ = NULL;
	son_ = NULL;
}

// -----------------------------------------------------------------------------
BIndexNode::~BIndexNode()			// destructor
{
	char* buf = NULL;
	if (dirty_) {					// if dirty, rewrite to disk
		int block_length = btree_->file_->get_blocklength();
		buf = new char[block_length];
		write_to_buffer(buf);
		btree_->file_->write_block(buf, block_);

		delete[] buf;buf = NULL;
	}

	if (key_) {						// release <key_>
		delete[] key_; key_ = NULL;
	}
	if (son_) {						// release <son_>
		delete[] son_; son_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void BIndexNode::init(				// init a new node, which not exist
	int level,							// level (depth) in b-tree
	BTree* btree)						// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = (char) level;			// init <level_>

	num_entries_ = 0;				// init <num_entries_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	dirty_ = true;					// init <dirty_>

									// init <capacity_>
	int b_length = btree_->file_->get_blocklength();
	capacity_ = (b_length - get_header_size()) / get_entry_size();
	if (capacity_ < 50) {			// ensure at least 50 entries
		printf("capacity = %d\n", capacity_);
		error("BIndexNode::init() capacity too small.\n", true);
	}

	key_ = new float[capacity_];	// init <key_>
	for (int i = 0; i < capacity_; i++) {
		key_[i] = MINREAL;
	}
	son_ = new int[capacity_];		// init <son_>
	for (int i = 0; i < capacity_; i++) {
		son_[i] = -1;
	}

	char* blk = new char[b_length];	// init <block_>, get new addr
	block_ = btree_->file_->append_block(blk);
	delete[] blk; blk = NULL;
}

// -----------------------------------------------------------------------------
void BIndexNode::init_restore(		// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>
	dirty_ = false;					// init <dirty_>

									// get block length
	int b_len = btree_->file_->get_blocklength();

									// init <capacity_>
	capacity_ = (b_len - get_header_size()) / get_entry_size();
	if (capacity_ < 50) {			// at least 50 entries
		printf("capacity = %d\n", capacity_);
		error("BIndexNode::init_restore capacity too small.\n", true);
	}

	key_ = new float[capacity_];	// init <key_>
	for (int i = 0; i < capacity_; i++) {
		key_[i] = MINREAL;
	}
	son_ = new int[capacity_];		// init <son_>
	for (int i = 0; i < capacity_; i++) {
		son_[i] = -1;
	}

	// -------------------------------------------------------------------------
	//  Read the buffer <blk> to init <level_>, <num_entries_>, <left_sibling_>,
	//  <right_sibling_>, <key_> and <son_>.
	// -------------------------------------------------------------------------
	char* blk = new char[b_len];
	btree_->file_->read_block(blk, block);
	read_from_buffer(blk);

	delete[] blk; blk = NULL;
}

// -----------------------------------------------------------------------------
//  entry: <key_>: SIZEFLOAT and <son_>: SIZEINT
// -----------------------------------------------------------------------------
int BIndexNode::get_entry_size()	// get entry size of b-node
{
	int entry_size = SIZEFLOAT + SIZEINT;
	return entry_size;
}

// -----------------------------------------------------------------------------
//  Read info from buffer to initialize <level_>, <num_entries_>,
//  <left_sibling_>, <right_sibling_>, <key_> and <son_> of b-index node
// -----------------------------------------------------------------------------
void BIndexNode::read_from_buffer(	// read a b-node from buffer
	char* buf)							// store info of a b-index node
{
	int i = 0;						// read <level_>
	memcpy(&level_, &buf[i], SIZECHAR);
	i += SIZECHAR;
									// read <num_entries_>
	memcpy(&num_entries_, &buf[i], SIZEINT);
	i += SIZEINT;
									// read <left_sibling_>
	memcpy(&left_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;
									// read <right_sibling_>
	memcpy(&right_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	for (int j = 0; j < num_entries_; j++) {
									// read <key_>
		memcpy(&key_[j], &buf[i], SIZEFLOAT);
		i += SIZEFLOAT;
									// read <son_>
		memcpy(&son_[j], &buf[i], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
void BIndexNode::write_to_buffer(	// write info of node into buffer
	char* buf)							// store info of this node (return)
{
	int i = 0;						// write <level_>
	memcpy(&buf[i], &level_, SIZECHAR);
	i += SIZECHAR;
									// write <num_entries_>
	memcpy(&buf[i], &num_entries_, SIZEINT);
	i += SIZEINT;
									// write <left_sibling_>
	memcpy(&buf[i], &left_sibling_, SIZEINT);
	i += SIZEINT;
									// write <right_sibling_>
	memcpy(&buf[i], &right_sibling_, SIZEINT);
	i += SIZEINT;

	for (int j = 0; j < num_entries_; j++) {
									// write <key_>
		memcpy(&buf[i], &key_[j], SIZEFLOAT);
		i += SIZEFLOAT;
									// write <son_>
		memcpy(&buf[i], &son_[j], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
//  Find position of entry that is just less than or equal to input entry.
//  If input entry is smaller than all entry in this node, we'll return -1.
//  The scan order is from right to left.
// -----------------------------------------------------------------------------
int BIndexNode::find_position_by_key(
	float key)							// input key
{
	int pos = -1;
									// linear scan (right to left)
	for (int i = num_entries_ - 1; i >= 0; i--) {
		if (key_[i] <= key) {
			pos = i;
			break;
		}
	}
	return pos;
}

// -----------------------------------------------------------------------------
float BIndexNode::get_key(			// get <key> indexed by <index>
	int index)							// input index
{
	if (index < 0 || index >= num_entries_) {
		error("BIndexNode::get_key out of range.", true);
	}
	return key_[index];
}

// -----------------------------------------------------------------------------
									// get the left-sibling node
BIndexNode* BIndexNode::get_left_sibling()
{
	BIndexNode* node = NULL;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BIndexNode();	// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
									// get the right-sibling node
BIndexNode* BIndexNode::get_right_sibling()
{
	BIndexNode* node = NULL;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BIndexNode();	// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BIndexNode::get_son(			// get son indexed by <index>
	int index)							// input index
{
	if (index < 0 || index >= num_entries_) {
		error("BIndexNode::get_son out of range.", true);
	}
	return son_[index];
}

// -----------------------------------------------------------------------------
void BIndexNode::add_new_child(		// add a new entry from its child node
	float key,							// input key
	int son)							// input son
{
	if (num_entries_ >= capacity_) {
		error("BIndexNode::add_new_child overflow", true);
	}

	key_[num_entries_] = key;		// add new entry into its pos
	son_[num_entries_] = son;

	num_entries_++;					// update <num_entries_>
	dirty_ = true;					// node modified, <dirty_> is true
}



// -----------------------------------------------------------------------------
//  BLeafNode: structure of leaf node in b-tree
// -----------------------------------------------------------------------------
BLeafNode::BLeafNode()				// constructor
{
	level_ = -1;
	num_entries_ = -1;
	left_sibling_ = right_sibling_ = -1;

	block_ = capacity_ = -1;
	dirty_ = false;
	btree_ = NULL;

	num_keys_ = -1;
	capacity_keys_ = -1;
	key_ = NULL;
	id_ = NULL;
}

// -----------------------------------------------------------------------------
BLeafNode::~BLeafNode()				// destructor
{
	char* buf = NULL;
	if (dirty_) {					// if dirty, rewrite to disk
		int block_length = btree_->file_->get_blocklength();
		buf = new char[block_length];
		write_to_buffer(buf);
		btree_->file_->write_block(buf, block_);

		delete[] buf;buf = NULL;
	}

	if (key_) {						// release <key_>
		delete[] key_; key_ = NULL;
	}
	if (id_) {						// release <id_>
		delete[] id_; id_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void BLeafNode::init(				// init a new node, which not exist
	int level,							// level (depth) in b-tree
	BTree* btree)						// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = (char) level;			// init <level_>

	num_entries_ = 0;				// init <num_entries_>
	num_keys_ = 0;					// init <num_keys_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;				// init <right_sibling_>
	dirty_ = true;					// init <dirty_>

									// get block length
	int b_length = btree_->file_->get_blocklength();

	// -------------------------------------------------------------------------
	//  Init <capacity_keys_> and calc key size
	// -------------------------------------------------------------------------
	int key_size = get_key_size(b_length);
									// init <key>
	key_ = new float[capacity_keys_];
	for (int i = 0; i < capacity_keys_; i++) {
		key_[i] = MINREAL;
	}
									// calc header size
	int header_size = get_header_size();
									// calc entry size
	int entry_size = get_entry_size();	
									// init <capacity>
	capacity_ = (b_length - header_size - key_size) / entry_size;
	if (capacity_ < 100) {			// at least 100 entries
		printf("capacity = %d\n", capacity_);
		error("BLeafNode::init capacity too small.\n", true);
	}
	id_ = new int[capacity_];		// init <id>
	for (int i = 0; i < capacity_; i++) {
		id_[i] = -1;
	}

	char* blk = new char[b_length];	// init <block>
	block_ = btree_->file_->append_block(blk);
	delete[] blk; blk = NULL;
}

// -----------------------------------------------------------------------------
void BLeafNode::init_restore(		// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>
	dirty_ = false;					// init <dirty_>

									// get block length
	int b_length = btree_->file_->get_blocklength();

	// -------------------------------------------------------------------------
	//  Init <capacity_keys> and calc key size
	// -------------------------------------------------------------------------
	int key_size = get_key_size(b_length);
									// init <key>
	key_ = new float[capacity_keys_];
	for (int i = 0; i < capacity_keys_; i++) {
		key_[i] = MINREAL;
	}
									// calc header size
	int header_size = get_header_size();
									// calc entry size
	int entry_size = get_entry_size();	
									// init <capacity>
	capacity_ = (b_length - header_size - key_size) / entry_size;
	if (capacity_ < 100) {			// at least 100 entries
		printf("capacity = %d\n", capacity_);
		error("BLeafNode::init_store capacity too small.\n", true);
	}
	id_ = new int[capacity_];		// init <id>
	for (int i = 0; i < capacity_; i++) {
		id_[i] = -1;
	}

	// -------------------------------------------------------------------------
	//  Read the buffer <blk> to init <level_>, <num_entries_>, <left_sibling_>,
	//  <right_sibling_>, <num_keys_> <key_> and <id_>
	// -------------------------------------------------------------------------
	char* blk = new char[b_length];
	btree_->file_->read_block(blk, block);
	read_from_buffer(blk);

	delete[] blk; blk = NULL;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_entry_size()		// get entry size in b-node
{
	return SIZEINT;						// <id>: sizeof(int)
}

// -----------------------------------------------------------------------------
void BLeafNode::read_from_buffer(	// read a b-node from buffer
	char* buf)							// store info of a b-node
{
	int i = 0;
	// -------------------------------------------------------------------------
	//  Read header: <level_> <num_entries_> <left_sibling_> <right_sibling_>
	// -------------------------------------------------------------------------
	memcpy(&level_, &buf[i], SIZECHAR);
	i += SIZECHAR;

	memcpy(&num_entries_, &buf[i], SIZEINT);
	i += SIZEINT;

	memcpy(&left_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	memcpy(&right_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	// -------------------------------------------------------------------------
	//  Read keys: <num_keys_> and <key_>
	// -------------------------------------------------------------------------
	memcpy(&num_keys_, &buf[i], SIZEINT);
	i += SIZEINT;

	for (int j = 0; j < capacity_keys_; j++) {
		memcpy(&key_[j], &buf[i], SIZEFLOAT);
		i += SIZEFLOAT;
	}

	// -------------------------------------------------------------------------
	//  Read entries: <id_>
	// -------------------------------------------------------------------------
	for (int j = 0; j < num_entries_; j++) {
		memcpy(&id_[j], &buf[i], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
void BLeafNode::write_to_buffer(	// write a b-node into buffer
	char* buf)							// store info of a b-node
{
	int i = 0;
	// -------------------------------------------------------------------------
	//  Write header: <level_> <num_entries_> <left_sibling_> <right_sibling_>
	// -------------------------------------------------------------------------
	memcpy(&buf[i], &level_, SIZECHAR);
	i += SIZECHAR;

	memcpy(&buf[i], &num_entries_, SIZEINT);
	i += SIZEINT;

	memcpy(&buf[i], &left_sibling_, SIZEINT);
	i += SIZEINT;

	memcpy(&buf[i], &right_sibling_, SIZEINT);
	i += SIZEINT;

	// -------------------------------------------------------------------------
	//  Write keys: <num_keys_> and <key_>
	// -------------------------------------------------------------------------
	memcpy(&buf[i], &num_keys_, SIZEINT);
	i += SIZEINT;

	for (int j = 0; j < capacity_keys_; j++) {
		memcpy(&buf[i], &key_[j], SIZEFLOAT);
		i += SIZEFLOAT;
	}

	// -------------------------------------------------------------------------
	//  Write entries: <id_>
	// -------------------------------------------------------------------------
	for (int j = 0; j < num_entries_; j++) {
		memcpy(&buf[i], &id_[j], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
int BLeafNode::find_position_by_key(// find pos just less than input key
	float key)							// input key
{
	int pos = -1;
									// linear scan (right to left)
	for (int i = num_keys_ - 1; i >= 0; i--) {
		if (key_[i] <= key) {
			pos = i;				// position of corresponding id
			break;
		}
	}
	return pos;
}

// -----------------------------------------------------------------------------
float BLeafNode::get_key(			// get <key_> indexed by <index>
	int index)							// input <index>
{
	if (index < 0 || index >= num_keys_) {
		error("BLeafNode::get_key out of range.", true);
	}
	return key_[index];
}

// -----------------------------------------------------------------------------
									// get left-sibling node
BLeafNode* BLeafNode::get_left_sibling()
{
	BLeafNode* node = NULL;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BLeafNode();		// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
									// get right sibling node
BLeafNode* BLeafNode::get_right_sibling()
{
	BLeafNode* node = NULL;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BLeafNode();		// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_key_size(		// get key size of this node
	int _block_length)					// block length
{
	capacity_keys_ = (int) ceil((float) _block_length / INDEX_SIZE_LEAF_NODE);

	// -------------------------------------------------------------------------
	//  Array of <key_> with number <capacity_keys_> + <number_keys_> (SIZEINT)
	// -------------------------------------------------------------------------
	int key_size = capacity_keys_ * SIZEFLOAT + SIZEINT;
	return key_size;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_increment()		// get <increment>
{
	int entry_size = get_entry_size();
	int increment = INDEX_SIZE_LEAF_NODE / entry_size;

	return increment;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_num_keys()		// get <num_keys_>
{
	return num_keys_;
}

// -----------------------------------------------------------------------------
int BLeafNode::get_entry_id(		// get entry id indexed by <index>
	int index)							// input <index>
{
	if (index < 0 || index >= num_entries_) {
		error("BLeafNode::get_entry_id out of range.", true);
	}
	return id_[index];
}

// -----------------------------------------------------------------------------
void BLeafNode::add_new_child(		// add new child by input id and key
	int id,								// input object id
	float key)							// input key
{
	if (num_entries_ >= capacity_) {
		error("BLeafNode::add_new_child entry overflow", true);
	}

	id_[num_entries_] = id;			// add new id into its pos

	if ((num_entries_ * SIZEINT) % INDEX_SIZE_LEAF_NODE == 0) {
		if (num_keys_ >= capacity_keys_) {
			error("BLeafNode::add_new_child key overflow", true);
		}

		key_[num_keys_] = key;		// add new key into its pos
		num_keys_++;				// update <num_keys>
	}

	num_entries_++;					// update <num_entries>
	dirty_ = true;					// node modified, <dirty> is true
}


