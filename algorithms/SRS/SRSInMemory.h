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

#ifndef SRSINMEMORY_H_
#define SRSINMEMORY_H_

#include <stdio.h>
#include <vector>
#include <algorithm>

#include "ParamFile.h"
#include "RandGen.h"
#include "SRSCoverTree.h"
#include "Rawdata.h"

template<typename T>
struct res_pair_raw {
  int id;
  T dist;
  bool operator>(const res_pair_raw<T> &) const;
  bool operator>=(const res_pair_raw<T> &) const;
  bool operator==(const res_pair_raw<T> &) const;
  bool operator<=(const res_pair_raw<T> &) const;
  bool operator<(const res_pair_raw<T> &) const;
};

template<typename T>
bool res_pair_raw<T>::operator>(const res_pair_raw<T> &n) const {
  return (dist > n.dist);
}
template<typename T>
bool res_pair_raw<T>::operator>=(const res_pair_raw<T> &n) const {
  return (dist >= n.dist);
}
template<typename T>
bool res_pair_raw<T>::operator==(const res_pair_raw<T> &n) const {
  return (dist == n.dist);
}
template<typename T>
bool res_pair_raw<T>::operator<=(const res_pair_raw<T> &n) const {
  return (dist <= n.dist);
}
template<typename T>
bool res_pair_raw<T>::operator<(const res_pair_raw<T> &n) const {
  return (dist < n.dist);
}

template<>
bool res_pair_raw<long long>::operator>(
    const res_pair_raw<long long> &n) const {
  return (dist > n.dist);
}
template<>
bool res_pair_raw<long long>::operator>=(
    const res_pair_raw<long long> &n) const {
  return (dist >= n.dist);
}
template<>
bool res_pair_raw<long long>::operator==(
    const res_pair_raw<long long> &n) const {
  return (dist == n.dist);
}
template<>
bool res_pair_raw<long long>::operator<=(
    const res_pair_raw<long long> &n) const {
  return (dist <= n.dist);
}
template<>
bool res_pair_raw<long long>::operator<(
    const res_pair_raw<long long> &n) const {
  return (dist < n.dist);
}

template<>
bool res_pair_raw<double>::operator>(const res_pair_raw<double> &n) const {
  return (dist > n.dist);
}
template<>
bool res_pair_raw<double>::operator>=(const res_pair_raw<double> &n) const {
  return (dist >= n.dist);
}
template<>
bool res_pair_raw<double>::operator==(const res_pair_raw<double> &n) const {
  return (dist == n.dist);
}
template<>
bool res_pair_raw<double>::operator<=(const res_pair_raw<double> &n) const {
  return (dist <= n.dist);
}
template<>
bool res_pair_raw<double>::operator<(const res_pair_raw<double> &n) const {
  return (dist < n.dist);
}

template<typename T> struct type_name {
  static const char* name() {
    return "double";
  }  // fixme
};
template<> struct type_name<int> {
  static const char* name() {
    return "int";
  }
};
template<> struct type_name<float> {
  static const char* name() {
    return "float";
  }
};
template<> struct type_name<double> {
  static const char* name() {
    return "double";
  }
};
template<> struct type_name<long long> {
  static const char* name() {
    return "long long";
  }
};

template<typename T> struct type_format {
  static const char* format() {
    return "%s";
  }  // fixme
};
template<> struct type_format<int> {
  static const char* format() {
    return "%d";
  }
};
template<> struct type_format<float> {
  static const char* format() {
    return "%f";
  }
};
template<> struct type_format<double> {
  static const char* format() {
    return "%f";
  }
};
template<> struct type_format<long long> {
  static const char* format() {
    return "%lld";
  }
};

template<typename T>
class SRS_In_Memory {
 private:
  long long n;
  int d;
  int m;
  float * proj;
  Raw_data<T> * raw_data;
  char * index_path;
  char * data_type;
  SRS_Cover_Tree * index;

  void get_proj(int n, int d, T * source, float * proj, float * dest);
 public:
  SRS_In_Memory(char * index_path);
  virtual ~SRS_In_Memory();

  void build_index(long long n, int d, int m, char * ds_path);
  void restore_index();
  template<typename X>
  void knn_search(T * query, int k, int t, double thres,
                  std::vector<res_pair_raw<X> > & heap);
  int get_m() {
    return this->m;
  }
  char * get_type() {
    return this->data_type;
  }
};

template<typename T>
SRS_In_Memory<T>::SRS_In_Memory(char * index_path) {
  this->index_path = new char[100];
  strcpy(this->index_path, index_path);
  if (this->index_path[strlen(this->index_path) - 1] != '/')
    strcat(this->index_path, "/");

  this->d = -1;
  this->n = -1;
  this->m = -1;
  this->proj = NULL;
  this->raw_data = NULL;
  this->index = NULL;
  this->data_type = new char[10];
}

template<typename T>
SRS_In_Memory<T>::~SRS_In_Memory() {
  delete[] this->proj;
  delete[] this->data_type;
  delete[] this->index_path;
  delete this->raw_data;
  delete this->index;
}

template<typename T>
void SRS_In_Memory<T>::build_index(long long n, int d, int m, char * ds_path) {
  this->n = n;
  this->d = d;
  this->m = m;
  T * data = new T[d];
  this->proj = new float[m * d];
  for (int i = 0; i < m * d; ++i) {
    proj[i] = gaussian(0, 1);
  }
  float * proj_data = new float[n * m];
  FILE *dfp = fopen(ds_path, "r");
  char file_path[100];
  strcpy(file_path, index_path);
  strcat(file_path, "raw_data.dat");
  FILE * fp = fopen(file_path, "wb");

  int elem_cnt = 0, temp;
  long long point_cnt = 0;

  //read data
  while (!feof(dfp) && point_cnt < n) {
    fscanf(dfp, type_format<T>::format(), &data[elem_cnt]);
    elem_cnt++;
    if (elem_cnt == d) {  // generate projected points
      for (int i = 0; i < m; ++i) {
        float p = 0.0;
        for (int j = 0; j < d; ++j) {
          p += data[j] * proj[i * d + j];
        }
        proj_data[point_cnt * m + i] = p;
      }
      fwrite(data, sizeof(T), d, fp);
      elem_cnt = 0;
      point_cnt++;
      if (point_cnt % 50000 == 0) {
        fprintf(stderr, "\r%lld (%.3f\%)", point_cnt,
                (double) point_cnt / n * 100);
      }
    }
  }
  fprintf(stderr, "\r%lld (100.000\%)\n", point_cnt);
  fclose(dfp);
  delete[] data;
  fclose(fp);
  //build srs_cover_tree
  Proj_data * data_proj = new Proj_data(n, m, proj_data);
  this->index = new SRS_Cover_Tree(n, m, data_proj);
  //write tree out
  strcpy(file_path, this->index_path);
  strcat(file_path, "index");
  this->index->write_to_disk_compressed(file_path);

  //write para out
  strcpy(file_path, index_path);
  strcat(file_path, "para.txt");
  writeParamFile(file_path, n, d, m, -1, proj, type_name<T>::name());  // no B in MEM model
}

template<typename T>
void SRS_In_Memory<T>::restore_index() {
  int B;
  this->proj = readParamFile(this->index_path, this->n, this->d, this->m, B,
                             this->data_type);
  char file_path[100];
  strcpy(file_path, this->index_path);
  strcat(file_path, "raw_data.dat");
  this->raw_data = new Raw_data<T>(n, d, file_path);
  strcpy(file_path, this->index_path);
  strcat(file_path, "index");
  this->index = new SRS_Cover_Tree(file_path);
}

template<typename T>
void SRS_In_Memory<T>::get_proj(int n, int d, T * source, float * proj,
                                float * dest) {
  for (int i = 0; i < n; ++i) {
    float p = 0.0;
    for (int j = 0; j < d; ++j) {
      p += source[j] * proj[i * d + j];
    }
    dest[i] = p;
  }
}

template<typename T>
template<typename X>
void SRS_In_Memory<T>::knn_search(T * query, int k, int t, double thres,
                                  std::vector<res_pair_raw<X> > & heap) {
  float * q_proj = new float[m];
  get_proj(m, d, query, this->proj, q_proj);
  this->index->init_search(q_proj);
  heap.clear();
  heap.reserve(k);
  int count = 0;
  while (count < t) {
    res_pair cover_tree_res = this->index->increm_knn_search_compressed();
    count++;
    if (thres > 0 && heap.size() == k
        && (cover_tree_res.dist * cover_tree_res.dist
            > heap.front().dist * thres)) {  // 1st time test early-stop condition
      this->index->finish_search();
      return;
    }
    res_pair_raw<X> res = { cover_tree_res.id, raw_data->cal_squared_dist(
        cover_tree_res.id, query) };
    bool changed = false;
    if (heap.size() < k) {
      heap.push_back(res);
      std::push_heap(heap.begin(), heap.end());
      changed = true;
    } else if (res.dist < heap.front().dist) {  // update top-k heap
      std::pop_heap(heap.begin(), heap.end());
      heap.pop_back();
      heap.push_back(res);
      std::push_heap(heap.begin(), heap.end());
      changed = true;
    }
    if (thres > 0 && changed && heap.size() == k
        && (cover_tree_res.dist * cover_tree_res.dist
            > heap.front().dist * thres)) {  // 2nd time test early-stop condition
      this->index->finish_search();
      return;
    }
  }
  this->index->finish_search();
  return;
}

#endif /* SRSINMEMORY_H_ */
