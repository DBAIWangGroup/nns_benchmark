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

#include "SRSCoverTree.h"

struct search_node {
  float min_dist;
  CompressedTreeNode * node;
  int id;

  bool operator>(const search_node &) const;
  bool operator>=(const search_node &) const;
  bool operator==(const search_node &) const;
  bool operator<=(const search_node &) const;
  bool operator<(const search_node &) const;
};

bool search_node::operator>(const search_node &n) const {
  return min_dist < n.min_dist;
}
bool search_node::operator>=(const search_node &n) const {
  return min_dist <= n.min_dist;
}
bool search_node::operator==(const search_node &n) const {
  return min_dist == n.min_dist;
}
bool search_node::operator<=(const search_node &n) const {
  return min_dist >= n.min_dist;
}
bool search_node::operator<(const search_node &n) const {
  return min_dist > n.min_dist;
}

float base = 1.3;  // 2 in cover-tree paper

float il2 = 1. / log(base);

inline float dist_of_scale(int s) {
  return pow(base, s);
}

inline int get_scale(float d) {
  return (int) ceilf(il2 * log(d));
}

inline int min(int a, int b) {
  if (a <= b)
    return a;
  else
    return b;
}

inline float max(float a, float b) {
  if (a >= b)
    return a;
  else
    return b;
}

SRS_Cover_Tree::SRS_Cover_Tree(long long n, int d, Proj_data * data) {
  this->n = n;
  this->isVectorized = false;
  this->num_node = 0;
  this->data = data;
  this->d = d;
  this->root = NULL;
  this->heap = NULL;
  this->query = NULL;
  this->pool = NULL;

  this->construct();
  this->compressed_vectorization();
}

SRS_Cover_Tree::SRS_Cover_Tree(char * file_path) {
  read_from_disk_compressed(file_path);
}

SRS_Cover_Tree::~SRS_Cover_Tree() {
  free(pool);
  delete this->data;
}

TreeNode * SRS_Cover_Tree::new_node(int id) {
  TreeNode * node = new TreeNode;
  if (isVectorized == true) {
    // Cannot Change the tree node structure as it is Vectorized.
    // This version only support bulk loading index construction.
    fprintf(stderr,
            "Cannot new node after cover-tree is already vectorized.\n");
    return NULL;
  }
  node->pivot = id;
  node->max_dist = 0;
  node->nextSibling = NULL;
  node->firstChild = NULL;
  num_node++;
  return node;
}

float SRS_Cover_Tree::get_max(v_array<ds_node> set) {
  float max = -1.0;
  for (int i = 0; i < set.index; ++i) {
    if (max < set[i].dist.last()) {
      max = set[i].dist.last();
    }
  }
  return max;
}

void SRS_Cover_Tree::split(v_array<ds_node> &point_set,
                           v_array<ds_node> &far_set, int scale) {
  unsigned int new_index = 0;
  float thres = dist_of_scale(scale);
  for (int i = 0; i < point_set.index; i++) {
    if (point_set[i].dist.last() <= thres) {
      point_set[new_index++] = point_set[i];
    } else {
      push(far_set, point_set[i]);
    }
  }
  point_set.index = new_index;
}

void SRS_Cover_Tree::dist_split(v_array<ds_node>& point_set,
                                v_array<ds_node>& new_point_set, int new_pivot,
                                int scale) {
  unsigned int new_index = 0;
  float thres = dist_of_scale(scale);
  for (int i = 0; i < point_set.index; i++) {
    float new_dist = data->cal_dist(new_pivot, point_set[i].id);
    if (new_dist <= thres) {
      push(point_set[i].dist, new_dist);
      push(new_point_set, point_set[i]);
    } else {
      point_set[new_index++] = point_set[i];
    }
  }
  point_set.index = new_index;
}

TreeNode * SRS_Cover_Tree::batch_insert(int pivot, int max_scale, int top_scale,
                                        v_array<ds_node> &point_set,
                                        v_array<ds_node> &consumed_set,
                                        v_array<v_array<ds_node> > &stack) {
  if (point_set.index == 0) {
    return new_node(pivot);
  }

  float max_dist = get_max(point_set);
  int next_scale = min(max_scale - 1, get_scale(max_dist));
  if (next_scale < -100) {  // if points are with distance 0
    TreeNode * firstChild = new_node(pivot);
    while (point_set.index > 0) {
      TreeNode * temp_node = new_node(point_set.last().id);
      temp_node->nextSibling = firstChild;
      firstChild = temp_node;
      push(consumed_set, point_set.last());
      point_set.decr();
    }
    return firstChild;
  }

  v_array<ds_node> far = pop(stack);
  split(point_set, far, max_scale);
  TreeNode * child = batch_insert(pivot, next_scale, top_scale, point_set,
                                  consumed_set, stack);

  if (point_set.index == 0) {
    push(stack, point_set);
    point_set = far;
    return child;
  }

  TreeNode * node = new_node(pivot);
  v_array<ds_node> new_point_set = pop(stack);
  v_array<ds_node> new_consumed_set = pop(stack);
  while (point_set.index != 0) {
    int new_pivot = point_set.last().id;
    push(consumed_set, point_set.last());
    point_set.decr();

    dist_split(point_set, new_point_set, new_pivot, max_scale);
    dist_split(far, new_point_set, new_pivot, max_scale);
    TreeNode * new_child = batch_insert(new_pivot, next_scale, top_scale,
                                        new_point_set, new_consumed_set, stack);
    new_child->nextSibling = child;
    child = new_child;

    float thres = dist_of_scale(max_scale);
    for (int i = 0; i < new_point_set.index; ++i) {
      new_point_set[i].dist.decr();
      if (new_point_set[i].dist.last() <= thres) {
        push(point_set, new_point_set[i]);
      } else {
        push(far, new_point_set[i]);
      }
    }
    for (int i = 0; i < new_consumed_set.index; ++i) {
      new_consumed_set[i].dist.decr();
      push(consumed_set, new_consumed_set[i]);
    }
    new_point_set.index = 0;
    new_consumed_set.index = 0;
  }
  push(stack, new_point_set);
  push(stack, new_consumed_set);
  push(stack, point_set);
  point_set = far;

  node->max_dist = get_max(consumed_set);
  node->firstChild = child;
  return node;
}

void SRS_Cover_Tree::construct() {
  v_array<ds_node> point_set;
  v_array<ds_node> consumed_set;
  v_array<v_array<ds_node> > stack;

  float max_dist = -1.0;
  for (int i = 1; i < n; ++i) {  // take the first point as pivot
    ds_node temp;
    temp.id = i;
    push(temp.dist, data->cal_dist(0, i));
    push(point_set, temp);
    if (temp.dist.last() > max_dist) {
      max_dist = temp.dist.last();
    }
  }

  this->root = batch_insert(0, get_scale(max_dist), get_scale(max_dist),
                            point_set, consumed_set, stack);

  for (int i = 0; i < consumed_set.index; i++) {
    free(consumed_set[i].dist.elements);
  }
  free(consumed_set.elements);
  for (int i = 0; i < stack.index; i++) {
    free(stack[i].elements);
  }
  free(stack.elements);
  free(point_set.elements);
}

void SRS_Cover_Tree::init_search(float * query) {
  search_node node;
  node.min_dist = max(
      0,
      data->cal_dist(this->compressed_root->pivot, query)
          - this->compressed_root->max_dist);
  node.node = this->compressed_root;
  node.id = -1;

  this->query = query;
  this->heap = new std::vector<search_node>;
  heap->push_back(node);
  heap->reserve(n / 100);  // Avoid too many re-size of heap by reserving the heap size to be 1% of the total number of points.
}

res_pair SRS_Cover_Tree::increm_knn_search_compressed() {
  while (!heap->empty()) {
    search_node node = heap->front();
    std::pop_heap(heap->begin(), heap->end());
    heap->pop_back();
    if (node.node == NULL) {
      res_pair res = { node.id, node.min_dist };
      return res;
    } else {
      int * external_children = node.node->get_external_children();
      for (int i = 0; i < node.node->num_of_external_node; ++i) {
        search_node new_node;
        new_node.id = external_children[i];
        new_node.min_dist = data->cal_dist(new_node.id, query);
        new_node.node = NULL;
        heap->push_back(new_node);
        std::push_heap(heap->begin(), heap->end());
      }
      CompressedTreeNode * internal_children =
          node.node->get_internal_children();
      for (int i = 0; i < node.node->num_of_internal_node; ++i) {
        search_node new_node;
        new_node.node = &internal_children[i];
        new_node.min_dist = max(
            0,
            data->cal_dist(new_node.node->pivot, query)
                - new_node.node->max_dist);
        new_node.id = -1;
        heap->push_back(new_node);
        std::push_heap(heap->begin(), heap->end());
      }
    }
  }
  res_pair res = { -1, -1 };
  return res;
}

void SRS_Cover_Tree::finish_search() {
  delete this->heap;
  delete[] this->query;
}

void SRS_Cover_Tree::compressed_vectorization(CompressedTreeNode * new_node,
                                              TreeNode * node) {
  unsigned char internal_node = 0, external_node = 0;
  TreeNode * temp = node->firstChild;
  while (temp != NULL) {
    if (temp->firstChild == NULL) {
      external_node++;
    } else {
      internal_node++;
    }
    if (external_node == 256 || internal_node == 256) {
      fprintf(
          stderr,
          "ABORT: fanout of the tree exceed 256, change the type in Line 47-48 in SRSCoverTree.h\n");
      return;
    }
    temp = temp->nextSibling;
  }

  new_node->pivot = node->pivot;
  new_node->max_dist = node->max_dist;
  new_node->num_of_internal_node = internal_node;
  new_node->num_of_external_node = external_node;
  new_node->children = pool + next_available;
  next_available += sizeof(int) * external_node
      + sizeof(CompressedTreeNode) * internal_node;

  // external nodes
  temp = node->firstChild;
  int * external_children = new_node->get_external_children();
  for (int i = 0; i < external_node; ++i) {
    while (temp->firstChild != NULL) {
      temp = temp->nextSibling;
    }
    external_children[i] = temp->pivot;
    temp = temp->nextSibling;
  }

  // internal nodes
  temp = node->firstChild;
  CompressedTreeNode * internal_children = new_node->get_internal_children();
  for (int i = 0; i < internal_node; ++i) {
    while (temp->firstChild == NULL) {
      TreeNode * another_temp = temp;
      temp = temp->nextSibling;
      delete another_temp;
    }
    TreeNode * another_temp = temp->nextSibling;
    compressed_vectorization(&internal_children[i], temp);
    temp = another_temp;
  }
  delete node;
}

void SRS_Cover_Tree::compressed_vectorization() {
  pool = (char *) malloc(
      sizeof(CompressedTreeNode) * (num_node - n) + sizeof(int) * n);
  next_available = 0;
  CompressedTreeNode * new_root = (CompressedTreeNode *) pool;
  next_available += sizeof(CompressedTreeNode);
  compressed_vectorization(new_root, root);
  //delete root;
  compressed_root = new_root;
  isVectorized = true;
}

void SRS_Cover_Tree::write_to_disk_compressed(char * file_path) {
  char fileex[255];
  FILE* fp;

  // Write out tree as binary file.
  sprintf(fileex, "%s.bin", file_path);
  fp = fopen(fileex, "wb");

  fwrite(&num_node, sizeof(int), 1, fp);
  fwrite(&n, sizeof(long long), 1, fp);
  fwrite(&d, sizeof(int), 1, fp);

  fwrite(&pool, sizeof(CompressedTreeNode*), 1, fp);  // This is for offset.
  fwrite(pool, sizeof(CompressedTreeNode) * (num_node - n) + sizeof(int) * n, 1,
         fp);
  fwrite(data->data, sizeof(float), n * d, fp);

  fclose(fp);

}

// called by read_from_disk_compressed
void adjust_offset(CompressedTreeNode * node, char * tmppool, char * pool) {
  node->children = (char *) ((char *) pool
      + ((char *) node->children - (char*) tmppool));
  CompressedTreeNode * internal_children = node->get_internal_children();
  for (int i = 0; i < node->num_of_internal_node; ++i) {
    adjust_offset(&internal_children[i], tmppool, pool);
  }
}

void SRS_Cover_Tree::read_from_disk_compressed(char * file_path) {
  char fileex[255];
  FILE* fp;
  char* tmppool;

  sprintf(fileex, "%s.bin", file_path);
  fp = fopen(fileex, "rb");

  fread(&num_node, sizeof(int), 1, fp);
  fread(&n, sizeof(long long), 1, fp);
  fread(&d, sizeof(int), 1, fp);

  fread(&tmppool, sizeof(CompressedTreeNode*), 1, fp);
  pool = (char *) malloc(
      sizeof(CompressedTreeNode) * (num_node - n) + sizeof(int) * n);
  fread(pool, 1, sizeof(CompressedTreeNode) * (num_node - n) + sizeof(int) * n,
        fp);

  // adjust the point offset
  this->compressed_root = (CompressedTreeNode *) pool;
  adjust_offset(this->compressed_root, tmppool, pool);
  // read data
  this->data = new Proj_data(n, d, NULL);
  fread(this->data->data, sizeof(float), n * d, fp);

  fclose(fp);
}

void traverse(CompressedTreeNode * node, int my_height, int & max_height,
              int & max_fanout) {
  if (node == NULL) {  //leaf
    if (my_height > max_height) {
      max_height = my_height;
    }
    return;
  } else {  //internal node
    int * external_children = node->get_external_children();
    for (int i = 0; i < node->num_of_external_node; ++i) {
      traverse(NULL, my_height + 1, max_height, max_fanout);
    }
    CompressedTreeNode * internal_children = node->get_internal_children();
    for (int i = 0; i < node->num_of_internal_node; ++i) {
      traverse(&internal_children[i], my_height + 1, max_height, max_fanout);
    }
    if (node->num_of_internal_node + node->num_of_external_node > max_fanout) {
      max_fanout = node->num_of_internal_node + node->num_of_external_node;
    }
  }
}

void SRS_Cover_Tree::tree_stat() {
  int my_height = 0, max_height = 0, max_fanout = 0;
  traverse(this->compressed_root, my_height, max_height, max_fanout);
  printf("max height: %d\nmax fanout: %d\n", max_height, max_fanout);
}
