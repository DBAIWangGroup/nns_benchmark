#ifndef __BLOCK_FILE_H
#define __BLOCK_FILE_H


// -----------------------------------------------------------------------------
//  BlockFile: structure of reading and writing file for b-tree
// -----------------------------------------------------------------------------
class BlockFile {
public:
	FILE* fp_;						// file pointer
	char* file_name_;				// file name
	bool  new_flag_;				// specifies if this is a new file
	
	int block_length_;				// length of a block
	int act_block_;					// block num of fp position
	int num_blocks_;				// total num of blocks

	// -------------------------------------------------------------------------
	BlockFile(						// constructor
		char* name,						// file name
		int b_length);					// length of a block

	~BlockFile();					// destructor

	// -------------------------------------------------------------------------
									// write <bytes> of length <num>
	void put_bytes(char* bytes, int num)
	{ fwrite(bytes, num, 1, fp_); }
									// read <bytes> of length <num>
	void get_bytes(char* bytes, int num)
	{ fread(bytes, num, 1, fp_); }

	void seek_block(int bnum) 		// move <fp_> to the right with <bnum>
	{ fseek(fp_, (bnum - act_block_) * block_length_, SEEK_CUR); }

	// -------------------------------------------------------------------------
	bool file_new() 				// whether this block is modified?
	{ return new_flag_; }

	int get_blocklength()			// get block length
	{ return block_length_; }

	int get_num_of_blocks()			// get number of blocks
	{ return num_blocks_; }

	// -------------------------------------------------------------------------
	void fwrite_number(				// write a value (type int)
		int num);						// value to write

	int fread_number();				// read a value (type int)

	// -------------------------------------------------------------------------
	void read_header(				// fetches info in the first block
		char* header);					// excluding the header of blk file

	void set_header(				// writes the info in the first block
		char* header);					// excluding the header of blk file

	// -------------------------------------------------------------------------
	bool read_block(				// read a block <b> in the <pos>
		Block block,					// a block
		int index);						// pos of the block

	bool write_block(				// write a block <b> in the <pos>
		Block block,					// a block
		int index);						// pos of the block

	int append_block(				// append a block at the end of file
		Block block);					// a block

	bool delete_last_blocks(		// delete last <num> blocks
		int num);						// num of blocks to be deleted
};


#endif
