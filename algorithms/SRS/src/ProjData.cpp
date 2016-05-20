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

#include "ProjData.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

inline float cal_squared_dist_1(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]);
}

inline float cal_squared_dist_2(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]);
}

inline float cal_squared_dist_3(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]);
}

inline float cal_squared_dist_4(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3]);
}
inline float cal_squared_dist_5(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3])
      + (a[4] - b[4]) * (a[4] - b[4]);
}
inline float cal_squared_dist_6(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3])
      + (a[4] - b[4]) * (a[4] - b[4]) + (a[5] - b[5]) * (a[5] - b[5]);
}
inline float cal_squared_dist_7(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3])
      + (a[4] - b[4]) * (a[4] - b[4]) + (a[5] - b[5]) * (a[5] - b[5])
      + (a[6] - b[6]) * (a[6] - b[6]);
}
inline float cal_squared_dist_8(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3])
      + (a[4] - b[4]) * (a[4] - b[4]) + (a[5] - b[5]) * (a[5] - b[5])
      + (a[6] - b[6]) * (a[6] - b[6]) + (a[7] - b[7]) * (a[7] - b[7]);
}
inline float cal_squared_dist_9(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3])
      + (a[4] - b[4]) * (a[4] - b[4]) + (a[5] - b[5]) * (a[5] - b[5])
      + (a[6] - b[6]) * (a[6] - b[6]) + (a[7] - b[7]) * (a[7] - b[7])
      + (a[8] - b[8]) * (a[8] - b[8]);
}
inline float cal_squared_dist_10(float * a, float * b) {
  return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
      + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3])
      + (a[4] - b[4]) * (a[4] - b[4]) + (a[5] - b[5]) * (a[5] - b[5])
      + (a[6] - b[6]) * (a[6] - b[6]) + (a[7] - b[7]) * (a[7] - b[7])
      + (a[8] - b[8]) * (a[8] - b[8]) + (a[9] - b[9]) * (a[9] - b[9]);
}

Proj_data::Proj_data(long long n, int d, float * data) {
  this->n = n;
  this->d = d;
  if (data == NULL) {
    this->data = new float[n * d];
  } else {
    this->data = data;
  }
  return;
}

Proj_data::~Proj_data() {
  delete[] this->data;
}

float Proj_data::cal_squared_dist(long long id1, long long id2) {
  float * a = &data[d * id1];
  float * b = &data[d * id2];
  switch (d) {
    case 6:
      return cal_squared_dist_6(a, b);
      break;
    case 7:
      return cal_squared_dist_7(a, b);
      break;
    case 8:
      return cal_squared_dist_8(a, b);
      break;
    case 9:
      return cal_squared_dist_9(a, b);
      break;
    case 10:
      return cal_squared_dist_10(a, b);
      break;
    case 1:
      return cal_squared_dist_1(a, b);
      break;
    case 2:
      return cal_squared_dist_2(a, b);
      break;
    case 3:
      return cal_squared_dist_3(a, b);
      break;
    case 4:
      return cal_squared_dist_4(a, b);
      break;
    case 5:
      return cal_squared_dist_5(a, b);
      break;
    default:
      break;
  }
  float res = 0.0, diff0, diff1, diff2, diff3;
  int iter = d / 4, last = d % 4;
  for (int i = 0; i < iter; ++i) {
    diff0 = a[0] - b[0];
    diff1 = a[1] - b[1];
    diff2 = a[2] - b[2];
    diff3 = a[3] - b[3];
    res += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
    a += 4;
    b += 4;
  }
  for (int i = 0; i < last; ++i) {
    diff0 = *a++ - *b++;
    res += diff0 * diff0;
  }
  return res;
}

float Proj_data::cal_squared_dist(long long id, float * q) {
  float * a = &data[d * id];
  switch (d) {
    case 6:
      return cal_squared_dist_6(a, q);
      break;
    case 7:
      return cal_squared_dist_7(a, q);
      break;
    case 8:
      return cal_squared_dist_8(a, q);
      break;
    case 9:
      return cal_squared_dist_9(a, q);
      break;
    case 10:
      return cal_squared_dist_10(a, q);
      break;
    case 1:
      return cal_squared_dist_1(a, q);
      break;
    case 2:
      return cal_squared_dist_2(a, q);
      break;
    case 3:
      return cal_squared_dist_3(a, q);
      break;
    case 4:
      return cal_squared_dist_4(a, q);
      break;
    case 5:
      return cal_squared_dist_5(a, q);
      break;
    default:
      break;
  }
  float res = 0.0, diff0, diff1, diff2, diff3;
  int iter = d / 4, last = d % 4;
  for (int i = 0; i < iter; ++i) {
    diff0 = a[0] - q[0];
    diff1 = a[1] - q[1];
    diff2 = a[2] - q[2];
    diff3 = a[3] - q[3];
    res += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
    a += 4;
    q += 4;
  }
  for (int i = 0; i < last; ++i) {
    diff0 = *a++ - *q++;
    res += diff0 * diff0;
  }
  return res;
}

float Proj_data::cal_dist(long long id1, long long id2) {
  float * a = &data[d * id1];
  float * b = &data[d * id2];
  switch (d) {
    case 6:
      return sqrt(cal_squared_dist_6(a, b));
      break;
    case 7:
      return sqrt(cal_squared_dist_7(a, b));
      break;
    case 8:
      return sqrt(cal_squared_dist_8(a, b));
      break;
    case 9:
      return sqrt(cal_squared_dist_9(a, b));
      break;
    case 10:
      return sqrt(cal_squared_dist_10(a, b));
      break;
    case 1:
      return sqrt(cal_squared_dist_1(a, b));
      break;
    case 2:
      return sqrt(cal_squared_dist_2(a, b));
      break;
    case 3:
      return sqrt(cal_squared_dist_3(a, b));
      break;
    case 4:
      return sqrt(cal_squared_dist_4(a, b));
      break;
    case 5:
      return sqrt(cal_squared_dist_5(a, b));
      break;
    default:
      break;
  }

  float res = 0.0, diff0, diff1, diff2, diff3;
  int iter = d / 4, last = d % 4;
  for (int i = 0; i < iter; ++i) {
    diff0 = a[0] - b[0];
    diff1 = a[1] - b[1];
    diff2 = a[2] - b[2];
    diff3 = a[3] - b[3];
    res += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
    a += 4;
    b += 4;
  }
  for (int i = 0; i < last; ++i) {
    diff0 = *a++ - *b++;
    res += diff0 * diff0;
  }
  return sqrt(res);
}

float Proj_data::cal_dist(long long id, float * q) {
  float * a = &data[d * id];
  switch (d) {
    case 6:
      return sqrt(cal_squared_dist_6(a, q));
      break;
    case 7:
      return sqrt(cal_squared_dist_7(a, q));
      break;
    case 8:
      return sqrt(cal_squared_dist_8(a, q));
      break;
    case 9:
      return sqrt(cal_squared_dist_9(a, q));
      break;
    case 10:
      return sqrt(cal_squared_dist_10(a, q));
      break;
    case 1:
      return sqrt(cal_squared_dist_1(a, q));
      break;
    case 2:
      return sqrt(cal_squared_dist_2(a, q));
      break;
    case 3:
      return sqrt(cal_squared_dist_3(a, q));
      break;
    case 4:
      return sqrt(cal_squared_dist_4(a, q));
      break;
    case 5:
      return sqrt(cal_squared_dist_5(a, q));
      break;
    default:
      break;
  }
  float res = 0.0, diff0, diff1, diff2, diff3;
  int iter = d / 4, last = d % 4;
  for (int i = 0; i < iter; ++i) {
    diff0 = a[0] - q[0];
    diff1 = a[1] - q[1];
    diff2 = a[2] - q[2];
    diff3 = a[3] - q[3];
    res += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
    a += 4;
    q += 4;
  }
  for (int i = 0; i < last; ++i) {
    diff0 = *a++ - *q++;
    res += diff0 * diff0;
  }
  return sqrt(res);
}

void Proj_data::print_row(long long index) {
  for (int i = 0; i < d; ++i) {
    printf("%f ", data[index * d + i]);
  }
  printf("\n");
}
