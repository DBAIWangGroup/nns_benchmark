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

#include "ParamFile.h"

#include <stdio.h>
#include <string.h>

void writeParamFile(char * file_path, long long n, int d, int m, int B,
                    float * proj, const char * type) {
  FILE * fp = fopen(file_path, "w");

  fprintf(fp, "B = %d\n", B);
  fprintf(fp, "n = %ld\n", n);
  fprintf(fp, "d = %d\n", d);
  fprintf(fp, "m = %d\n", m);

  for (int i = 0; i < m * d; i++) {
    fprintf(fp, "%f ", proj[i]);
  }
  fprintf(fp, "\n%s s", type);

  fclose(fp);
}

float * readParamFile(char * index_path, long long &n, int &d, int &m, int &B,
                      char * type) {
  char file_path[100];
  strcpy(file_path, index_path);
  strcat(file_path, "para.txt");
  FILE *fp = fopen(file_path, "r");

  fscanf(fp, "B = %d\n", &B);
  fscanf(fp, "n = %ld\n", &n);
  fscanf(fp, "d = %d\n", &d);
  fscanf(fp, "m = %d\n", &m);

  float *proj = new float[m * d];
  for (int i = 0; i < m * d; i++) {
    fscanf(fp, "%f", &proj[i]);
  }

  fscanf(fp, "\n");
  int i = 0;
  ;
  fgets(type, 100, fp);  //, "%s", type);
  for (; type[i] != ' '; ++i) {
  }
  type[i] = 0;

  fclose(fp);
  return proj;
}
