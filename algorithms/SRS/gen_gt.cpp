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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <queue>

#define no_argument 0
#define required_argument 1
#define optional_argument 2

//#define _LARGEFILE_SOURCE
//#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#define O_LARGEFILE 0

#define MAXINT 1e9

typedef std::priority_queue<double> max_queue_t;

void usage();
bool file_exists(const char *);
double l2_dist(int *_p1, int *_p2, int _dim);
double l2_dist(float *_p1, float *_p2, int _dim);
void gen_gt(int, int, int, const char *, const char *, const char *,
            bool is_integer);

int main(int argc, char * argv[]) {
  const struct option longopts[] ={
    {"dimension",                   required_argument, 0, 'd'},
    {"dataset-file-path",           required_argument, 0, 's'},
    {"cardinality",                 required_argument, 0, 'n'},
    {"k",                           required_argument, 0, 'k'},
    {"ground-truth-file-path",      required_argument, 0, 'g'},
    {"query-file-path",             required_argument, 0, 'q'},
    {"data-type",                   required_argument, 0, 'y'},
    {0, 0, 0, 0},
  };

  int index;
  int iarg = 0;
  opterr = 1;    //getopt error message (off: 0)

  int d = -1;
  int n = -1;
  int k = -1;
  bool is_integer = true;

  char ground_truth_file_path[100] = "";
  char query_file_path[100] = "";
  char data_file_path[100] = "";

  bool is_valid_command = true;

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, "d:g:k:n:q:s:y:h", longopts, &index);

    switch (iarg) {
      case 'd':
        if (optarg) {
          d = atoi(optarg);
        }
        break;
      case 'g':
        if (optarg) {
          strcpy(ground_truth_file_path, optarg);
        }
        break;
      case 'h':
        usage();
        return 0;
      case 'k':
        if (optarg) {
          k = atoi(optarg);
        }
        break;
      case 'n':
        if (optarg) {
          n = atoi(optarg);
        }
        break;
      case 'q':
        if (optarg) {
          strcpy(query_file_path, optarg);
        }
        break;
      case 's':
        if (optarg) {
          strcpy(data_file_path, optarg);
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

  if (d < 0 || k < 0 || n < 0 || !file_exists(data_file_path)
      || !file_exists(query_file_path)) {
    is_valid_command = false;
  }
  if (!is_valid_command) {
    usage();
  } else {
    gen_gt(n, d, k, data_file_path, query_file_path, ground_truth_file_path,
           is_integer);
  }

  return 0;
}

void usage() {
  printf("SRS-Mem (v1.0)\n");
  printf("Options\n");
  printf("-d {value}\tdimensionality of data\n");
  printf("-g {string}\tground truth file\n");
  printf("-k {value}\tnumber of neighbors wanted\n");
  printf("-n {value}\tcardinality\n");
  printf("-q {string}\tquery file\n");
  printf("-s {string}\tdataset file\n");
  printf(
      "-y {string}\tdata type (i: integer; f: floating number), default value: integer\n");
  printf("\n");

  printf("Usage:\n");

  printf("Generate ground truth file\n");
  printf("-d -g -k -n -q -s [-y]\n");
}

bool file_exists(const char *filename) {
  std::ifstream ifile(filename);
  return ifile;
}

double l2_dist_int(int *_p1, int *_p2, int _dim) {
  double ret = 0;
  for (int i = 0; i < _dim; i++) {
    long long dif = (_p1[i] - _p2[i]);
    ret += dif * dif;
  }
  return ret;
}

double l2_dist_float(float *_p1, float *_p2, int _dim) {
  double ret = 0;
  for (int i = 0; i < _dim; i++) {
    double dif = (_p1[i] - _p2[i]);
    ret += dif * dif;
  }
  return ret;
}

void gen_gt(int n, int d, int k, const char * data_file,
            const char * query_file, const char * gt_file, bool is_integer) {
  FILE * dfp = fopen(data_file, "r");
  FILE * qfp = fopen(query_file, "r");
  FILE * rfp = fopen(gt_file, "w+");

  if (dfp == NULL || qfp == NULL || rfp == NULL) {
    printf("open file error\n");
  }

  int nq;
  fscanf(qfp, "%d %d", &nq, &d);
  fprintf(rfp, "%d %d\n", nq, k);

  int * qs_int = new int[d * nq];
  int * data_int = new int[d];
  float * qs_float = new float[d * nq];
  float * data_float = new float[d];

  int temp = -1;
  double dist = -1;

  for (int i = 0; i < nq; ++i) {
    fscanf(qfp, "%d", &temp);
    for (int j = 0; j < d; ++j) {
      if (is_integer) {
        fscanf(qfp, "%d", &qs_int[i * d + j]);
      } else {
        fscanf(qfp, "%f", &qs_float[i * d + j]);
      }
    }
  }

  max_queue_t *max_queue_list = new max_queue_t[nq];
  for (int i = 0; i < n; ++i) {
    if (i % 5000 == 0) {
      fprintf(stderr, "\r%d (%.3f\%)", i, 1.0 * i / n * 100);
    }
    for (int j = 0; j < d; ++j) {
      if (is_integer) {
        fscanf(dfp, "%d", &data_int[j]);
      } else {
        fscanf(dfp, "%f", &data_float[j]);
      }
    }

    for (int j = 0; j < nq; ++j) {
      if (is_integer) {
        dist = l2_dist_int(data_int, &qs_int[j * d], d);
      } else {
        dist = l2_dist_float(data_float, &qs_float[j * d], d);
      }
      if (max_queue_list[j].size() < k) {
        max_queue_list[j].push(dist);
      } else if (max_queue_list[j].top() > dist) {
        max_queue_list[j].pop();
        max_queue_list[j].push(dist);
      }
    }
  }
  fprintf(stderr, "\r%d (%.3f\%)\n", n, 100.0);

  double *klist = new double[k];
  for (int i = 0; i < nq; ++i) {
    fprintf(rfp, "%d", i + 1);
    while (max_queue_list[i].size() > 0) {
      klist[max_queue_list[i].size() - 1] = max_queue_list[i].top();
      max_queue_list[i].pop();
    }
    for (int j = 0; j < k; ++j) {
      fprintf(rfp, " %f", sqrt(klist[j]));
    }
    fprintf(rfp, "\n");
  }

  fclose(dfp);
  fclose(qfp);
  fclose(rfp);

  delete[] qs_int;
  delete[] data_int;
  delete[] qs_float;
  delete[] data_float;
  delete[] max_queue_list;
  delete[] klist;
}
