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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "RandGen.h"

void usage();
//void gen_hard_data_integer(int n, int d, float c, char * data_file_path,
//                           char * query_file_path);
void gen_hard_data_float(int n, int d, float c, char * data_file_path,
                         char * query_file_path);

int main(int argc, char* argv[]) {
  const struct option longopts[] = {
        { "help",                 no_argument,       0, 'h' },
        { "dimension",            required_argument, 0, 'd' },
        { "cardinality",          required_argument, 0, 'n' },
        { "approximation-ratio",  required_argument, 0, 'c' },
        { "data-file-path",       required_argument, 0, 's' },
        { "query-file-path",      required_argument, 0, 'q' },
        { "data-type",            required_argument, 0, 'y' },
        { 0, 0, 0, 0 },
  };

  int index;
  int iarg = 0;
  opterr = 1;    //getopt error message (off: 0)

  int d = -1;
  int n = -1;
  float c = 4.0;
  bool is_integer = false;

  char query_file_path[100] = "";
  char data_file_path[100] = "";

  bool is_valid_command = true;

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, "c:d:n:q:s:y:h", longopts, &index);

    switch (iarg) {
      case 'd':
        if (optarg) {
          d = atoi(optarg);
        }
        break;
      case 'c':
        if (optarg) {
          c = atof(optarg);
        }
        break;
      case 'h':
        usage();
        return 0;
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
          if (strcmp(optarg, "i") == 0) {
            is_integer = true;
	    fprintf(stderr, "currently do not support integer coordinates, please use '-y f'\n"); 
          } else if (strcmp(optarg, "f") != 0) {
            is_valid_command = false;
          }
        }
        break;
    }
  }

  if (d <= 0 || n <= 0 || c <= 1.0) {
    is_valid_command = false;
  }
  if (!is_valid_command) {
    usage();
    return 0;
  }

  gen_hard_data_float(n, d, c, data_file_path, query_file_path);

  return 1;
}

void gen_hard_data_float(int n, int d, float c, char * data_file_path,
                         char * query_file_path) {
  //generate query point and write into query file
  float * query = new float[d];
  FILE * fp = fopen(query_file_path, "w+");
  fprintf(fp, "%d %d\n", 1, d);
  for (int i = 0; i < d; ++i) {
    query[i] = uniform(0, 1);
    fprintf(fp, "%f ", query[i]);
  }
  fprintf(fp, "\n");
  fclose(fp);

  //generate query's NN (distance to q is 1) as the first data point
  float * nn = new float[d];
  fp = fopen(data_file_path, "w+");
  float norm_factor = 0.0;
  for (int i = 0; i < d; ++i) {
    nn[i] = gaussian(0, 1);
    norm_factor += nn[i] * nn[i];
  }
  norm_factor = sqrt(norm_factor);
  for (int i = 0; i < d; ++i) {
    fprintf(fp, "%f ", nn[i] / norm_factor + query[i]);
  }
  fprintf(fp, "\n");

  //generate other c+\epsilon NNs and write into query file
  float * cnn = new float[d];
  for (int j = 1; j < n; ++j) {
    norm_factor = 0.0;
    for (int i = 0; i < d; ++i) {
      cnn[i] = gaussian(0, 1);
      norm_factor += cnn[i] * cnn[i];
    }
    norm_factor = sqrt(norm_factor) / (c + 0.001);
    for (int i = 0; i < d; ++i) {
      fprintf(fp, "%f ", cnn[i] / norm_factor + query[i]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);

  delete cnn;
  delete query;
  delete nn;
}

/*
void gen_hard_data_integer(int n, int d, float c, char * data_file_path,
                           char * query_file_path) {
  //generate query point and write into query file
  int * query = new int[d];
  FILE * fp = fopen(query_file_path, "w+");
  fprintf(fp, "%d %d\n", 1, d);
  for (int i = 0; i < d; ++i) {
    query[i] = (int) (uniform(0, 1) * 1000000);
    fprintf(fp, "%d ", query[i]);
  }
  fprintf(fp, "\n");
  fclose(fp);

  //generate query's NN (distance to q is 1) as the first data point
  float * nn = new float[d];
  fp = fopen(data_file_path, "w+");
  float norm_factor = 0.0;
  for (int i = 0; i < d; ++i) {
    nn[i] = gaussian(0, 1);
    norm_factor += nn[i] * nn[i];
  }
  norm_factor = sqrt(norm_factor);
  for (int i = 0; i < d; ++i) {
    fprintf(fp, "%d ", (int) (nn[i] / norm_factor * 1000000) + query[i]);
  }
  fprintf(fp, "\n");

  //generate other c+\epsilon NNs and write into query file
  float * cnn = new float[d];
  for (int j = 1; j < n; ++j) {
    norm_factor = 0.0;
    for (int i = 0; i < d; ++i) {
      cnn[i] = gaussian(0, 1);
      norm_factor += cnn[i] * cnn[i];
    }
    norm_factor = sqrt(norm_factor) / (c + 0.001);
    for (int i = 0; i < d; ++i) {
      fprintf(fp, "%d ", (int) (cnn[i] / norm_factor * 1000000) + query[i]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);

  delete cnn;
  delete query;
  delete nn;
}
*/

void usage() {
  printf("SRS-Mem (v1.0)\n");
  printf("Options\n");
  printf("-c {value}\tapproximation ratio (>= 1), default value: 4\n");
  printf("-d {value}\tdimensionality of data\n");
  printf("-n {value}\tcardinality\n");
  printf("-q {string}\tquery file\n");
  printf("-s {string}\tdataset file\n");
  //printf("-y {string}\tdata type (i: integer; f: floating number), default value: integer\n");
  printf("\n");

  printf("Usage:\n");

  printf("Generate worst case data set\n");
  //printf("[-c] -d -n -q -s [-y]\n");
  printf("[-c] -d -n -q -s\n");
}

