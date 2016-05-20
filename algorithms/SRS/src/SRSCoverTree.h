/*
 *   This file is part of SRS project.
 *
 *   SRS is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   SRS is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with SRS. If not, see <http://www.gnu.org/licenses/>.
 *
 *   Created by: Yifang Sun, Jianbin Qin
 *   Last modified by: Yifang Sun, Jianbin Qin
 */

/*  We implement some of the functions based on the implementation of cover-tree project
 *   (url: http://hunch.net/~jl/projects/cover_tree/cover_tree.html)
 */

#ifndef SRSCOVERTREE_H_
#define SRSCOVERTREE_H_

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ProjData.h"
#include "stack.h"

struct TreeNode {
  int pivot;  // id
  float max_dist;  //maximum distance to all grandchildren
  TreeNode * firstChild;  // for firstChild-nextSibling structure
  TreeNode * nextSibling;  // for firstChild-nextSibling structure
};

struct CompressedTreeNode {
  int pivot;  // id
  float max_dist;  // maximum distance to all descendants
  unsigned char num_of_external_node;
  unsigned char num_of_internal_node;
  char * children;
  CompressedTreeNode * get_internal_children() {
    // extenral node and then internal node
    return (CompressedTreeNode *) (children + sizeof(int) * num_of_external_node);
  }
  int * get_external_children() {
    return (int *) children;
  }
};

struct res_pair {
  int id;
  float dist;
};

struct ds_node {
  int id;
  v_array<float> dist;
};

struct search_node;

class SRS_Cover_Tree {
 private:
  /****index****/
  TreeNode * root;
  CompressedTreeNode * compressed_root;
  int num_node;
  //TreeNode *node_pool; // to be deleted
  char *pool;
  long long next_available;
  bool isVectorized;
  long long n;
  int d;
  Proj_data * data;

  /****query****/
  float * query;
  std::vector<search_node> * heap;

  TreeNode * batch_insert(int pivot, int max_scale, int top_scale,
                          v_array<ds_node> &point_set,
                          v_array<ds_node> &consumed_set,
                          v_array<v_array<ds_node> > &stack);  // batch insert a set of points into the tree.
  TreeNode * new_node(int id);
  void construct();
  void compressed_vectorization(CompressedTreeNode * new_node, TreeNode * node);
  void compressed_vectorization();  // change the tree structure (from firstChild-nextSibling to children list)and do memory optimization to improve query performance
  void read_from_disk_compressed(char * file_path);

  void dist_split(v_array<ds_node>& point_set, v_array<ds_node>& new_point_set,
                  int new_pivot, int scale);  // split points from point_set and new_point_set according to their distance to the pivot
  float get_max(v_array<ds_node> set);  // get the diagram of the set
  void split(v_array<ds_node> &point_set, v_array<ds_node> &far_set, int scale);  // split points from point_set according to their distance

 public:
  SRS_Cover_Tree(long long n, int d, Proj_data * data);  // construct tree from dataset
  SRS_Cover_Tree(char * file_path);  // restore tree from disk
  virtual ~SRS_Cover_Tree();
  void search_knn(float * query, int k);  // knn search on cover tree
  void init_search(float * query);
  res_pair increm_knn_search_compressed();  // incremental knn search on compressed cover tree
  void finish_search();

  void write_to_disk_compressed(char * file_path);
  void tree_stat();  // for test only
};

#endif /* SRSCOVERTREE_H_ */

