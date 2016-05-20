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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <getopt.h>
#include <cstdlib>
#include <dirent.h>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <set>
#include <boost/math/distributions/chi_squared.hpp>
#include "SRSInMemory.h"

#define no_argument 0
#define required_argument 1
#define optional_argument 2

using namespace std;

void usage();
bool file_exists(const char *);
bool dir_exists(const char *foldername);
double cal_thres(double c, double p_thres, int m);

template<class T>
void query_workload(SRS_In_Memory<T> * searcher, int nq, int d, int k, int t, double thres,
                    char *query_file_path, char *ground_truth_file_path,
                    char *output_file_path);

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main(int argc, char * argv[]) {
  const struct option longopts[] ={
    {"help",                        no_argument,       0, 'h'},
    {"page-size",                   required_argument, 0, 'b'},
    {"approximation-ratio",         required_argument, 0, 'c'},
    {"dimension",                   required_argument, 0, 'd'},
    {"seed",                        required_argument, 0, 'e'},
    {"ground-truth-file-path",      required_argument, 0, 'g'},
    {"index-dir-path",              required_argument, 0, 'i'},
    {"is-index",                    no_argument,       0, 'I'},
    {"k",                           required_argument, 0, 'k'},
    {"m",                           required_argument, 0, 'm'},
    {"cardinality",                 required_argument, 0, 'n'},
    {"output-file-path",            required_argument, 0, 'o'},
    {"query-file-path",             required_argument, 0, 'q'},
    {"is-query",                    no_argument,       0, 'Q'},
    {"threshold",                   required_argument, 0, 'r'},
    {"dataset-file-path",           required_argument, 0, 's'},
    {"max-number-of-points",        required_argument, 0, 't'},
    {"data-type",                   required_argument, 0, 'y'},
    {"query-size",                  required_argument, 0, 'p'},
    {0, 0, 0, 0},
  };

  srand(1000);
  int index;
  int iarg = 0;
  opterr = 1;    //getopt error message (off: 0)

  int d = -1;
  long long n = -1;
  int k = -1;
  int b = -1;
  int m = -1;
  int t = -1;
  int nq =-1;

  double c = -1.0;
  double p_thres = -1.0;

  char ground_truth_file_path[100] = "";
  char query_file_path[100] = "";
  char data_file_path[100] = "";
  char output_file_path[100] = "";  //output file, currently not used.
  char index_dir_path[100] = "";
  char data_type[100] = "";
  char result_file_path[100] = "";

  bool is_valid_command = true;
  bool is_index = false;
  bool is_query = false;
  bool is_integer = true;

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, "b:c:d:e:g:i:k:m:n:o:q:R:r:s:t:y:p:hIQ",
                       longopts, &index);

    switch (iarg) {
      case 'b':
        if (optarg) {
          b = atoi(optarg);
        }
        break;
      case 'c':
        if (optarg) {
          c = atof(optarg);
        }
        break;
      case 'd':
        if (optarg) {
          d = atoi(optarg);
        }
        break;
      case 'e':
        if (optarg) {
          srand (atoi(optarg));}
          break;
          case 'g':
          if (optarg) {
            strcpy(ground_truth_file_path, optarg);
          }
          break;
          case 'h':
          usage();
          return 0;
          case 'i':
          if (optarg) {
            strcpy(index_dir_path, optarg);
          }
          break;
          case 'I':
          is_index = true;
          break;
          case 'k':
          if (optarg) {
            k = atoi(optarg);
          }
          break;
          case 'm':
          if (optarg) {
            m = atoi(optarg);
          }
          break;
          case 'n':
          if (optarg) {
            n = atoi(optarg);
          }
          break;
		  case 'p':
          if (optarg) {
            nq = atoi(optarg);
          }
          break;
          case 'o':
          if (optarg) {
            strcpy(output_file_path, optarg);
          }
          break;
          case 'R':
          if (optarg) {
            strcpy(result_file_path, optarg);
          }
          break;
          case 'q':
          if (optarg) {
            strcpy(query_file_path, optarg);
          }
          break;
          case 'Q':
          is_query = true;
          break;
          case 'r':
          if (optarg) {
            p_thres = atof(optarg);
          }
          break;
          case 's':
          if (optarg) {
            strcpy(data_file_path, optarg);
          }
          break;
          case 't':
          if (optarg) {
            t = atoi(optarg);
          }
          break;
          case 'y':
          if (optarg) {
            if (strcmp(optarg, "f") == 0) {
              is_integer = false;
            } else if (strcmp(optarg, "i") != 0) {
              is_valid_command = false;
            }
          }
          break;

        }
      }

  if (is_index == is_query) {
    is_valid_command = false;
  }
  if (is_index) {
    if (!is_valid_command || d < 0 || m < 0 || n < 0
        || !file_exists(data_file_path) || !dir_exists(index_dir_path)) {
      is_valid_command = false;
    } else if (b < 0) {
      if (is_integer) {
        SRS_In_Memory<int> * indexer = new SRS_In_Memory<int>(index_dir_path);
        indexer->build_index(n, d, m, data_file_path);
        delete indexer;
      } else {
		timeval start;
    	gettimeofday(&start, NULL);
        SRS_In_Memory<float> * indexer = new SRS_In_Memory<float>(
            index_dir_path);
        indexer->build_index(n, d, m, data_file_path);
		timeval end;
    	gettimeofday(&end, NULL);
    	float index_time = diff_timeval(end, start);
		cout<<index_time<<" #indextime"<<endl;
        delete indexer;
      }
    } else {
      printf("use R-tree here\n");
    }
  }
  if (is_query) {
    if (!is_valid_command || k < 0 || c < 1 || t < 0 || p_thres < 0
        || !file_exists(query_file_path) || !file_exists(ground_truth_file_path)
        || !dir_exists(index_dir_path)) {
      is_valid_command = false;
    } else {
      char * type = new char[10];
      float * temp = readParamFile(index_dir_path, n, d, m, b, type);
      delete[] temp;
      if (b == -1) {
        if (strcmp(type, "int") == 0) {
          SRS_In_Memory<int> * searcher = new SRS_In_Memory<int>(
              index_dir_path);
          searcher->restore_index();
          query_workload(searcher, nq, d ,k, t, cal_thres(c, p_thres, m),
                         query_file_path, ground_truth_file_path,
                         output_file_path);
          delete searcher;
        } else if (strcmp(type, "float") == 0) {
          SRS_In_Memory<float> * searcher = new SRS_In_Memory<float>(
              index_dir_path);
          searcher->restore_index();
          query_workload(searcher, nq, d, k, t, cal_thres(c, p_thres, m),
                         query_file_path, ground_truth_file_path,
                         output_file_path);
          delete searcher;
        }
        delete[] type;
      } else {
        printf("use R-tree here\n");
      }
    }
  }

  if (!is_valid_command) {
    usage();
  }

  return 0;
}

void usage() {
  printf("SRS-Mem (v1.0)\n");
  printf("Options\n");
  //printf("-b {value}\tpage size in bytes (need to be a multiple of 4)\n");
  printf("-c {value}\tapproximation ratio (>= 1)\n");
  printf("-d {value}\tdimensionality of data\n");
  printf("-e {value}\tseed for random generators\n");
  printf("-g {string}\tground truth file\n");
  printf("-i {string}\tsrs index path (dir)\n");
  printf("-I (function)\tindex data\n");
  printf("-k {value}\tnumber of neighbors wanted\n");
  printf("-m {value}\tdimensionality of the projected space\n");
  printf("-n {value}\tcardinality\n");
  //printf("-o {string}\toutput file\n");
  printf("-q {string}\tquery file\n");
  printf("-Q (function)\tprocess queries\n");
  printf("-r {value}\tthreshold of early termination condition\n");
  printf("-s {string}\tdataset file\n");
  printf("-t {value}\tmaximum number of verify points\n");
  printf(
      "-y {string}\tdata type (i: integer; f: floating number), default value: integer\n");
  printf("\n");
  printf("Usage:\n");

  // printf("Index data (using R-tree)\n");
  // printf("-I -b -d -i -m -n -s\n");

  printf("Index data (using cover-tree)\n");
  printf("-I -d -i -m -n -s [-y]\n");

  printf("Process queries\n");
  printf("-Q -c -g -i -k -q -r -t\n");
}

bool file_exists(const char *filename) {
  std::ifstream ifile(filename);
  if (!ifile) {
    fprintf(stderr, "cannot open file %s\n", filename);
  }
  return ifile;
}

bool dir_exists(const char *dirname) {
  DIR * dir = opendir(dirname);
  if (dir) {
    closedir(dir);
    return true;
  }
  fprintf(stderr, "cannot open dir %s\n", dirname);
  return false;
}

template<class T>
void query_workload(SRS_In_Memory<T> * searcher,int qn, int d, int k, int t, double thres,
                    char *query_file_path, char *ground_truth_file_path,
                    char *output_file_path) {
  typedef typename Accumulator<T>::Type ResultType;
 
  FILE *qfp = fopen(query_file_path, "r");
  FILE *gfp = fopen(ground_truth_file_path, "r");
  FILE *ofp = fopen(output_file_path, "a+");

  double overall_recall = 0.0;
  double overall_time = 0.0;
  double overall_MAP = 0.0;

  T * query = new T[d];
  int * gt = new int[k];
  std::vector<res_pair_raw<ResultType> > res;
  for (int i = 0; i < qn; ++i) {
    for (int j = 0; j < d; ++j) {
      fscanf(qfp, type_format<T>::format(), &query[j]);
    }
    for (int j = 0; j < k; ++j) {
      fscanf(gfp, "%d ", &gt[j]);
    }

    timeval start;
    gettimeofday(&start, NULL);
    searcher->knn_search(query, k, t, thres, res);
    timeval end;
    gettimeofday(&end, NULL);
    overall_time += diff_timeval(end, start);

    int ratio = 0;
    std::sort(res.begin(), res.end());
	//compute recall
	std::set<int> gnd_row;
	for(unsigned j=0; j<k; j++)
		gnd_row.insert(gt[j]);
	for(unsigned j=0; j<k; j++)
	{
		if(gnd_row.find(res[j].id)!=gnd_row.end())
			ratio++;
	}

	overall_recall += ratio;
    }
	float recall = overall_recall/k/qn;
	float searchtime = overall_time / qn;
	fprintf(ofp,"%.6f %.6f #N_%d \n",recall,searchtime,t);

  
  delete[] query;
  delete[] gt;
  fclose(qfp);
  fclose(gfp);
  fclose(ofp);
}

double cal_thres(double c, double p_thres, int m) {
  if (p_thres >= 1) {
    return -1;
  }
  boost::math::chi_squared chi(m);
  return boost::math::quantile(chi, p_thres) / c / c;
}
