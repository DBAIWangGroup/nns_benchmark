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

#ifndef RAWDATA_H_
#define RAWDATA_H_

#include <type_traits>
#include <stdio.h>

template<typename T>
struct Accumulator {
  typedef T Type;
};
template<>
struct Accumulator<float> {
  typedef double Type;
};
template<>
struct Accumulator<int> {
  typedef long long Type;
};

template<class T>
class Raw_data {
  typedef typename Accumulator<T>::Type ResultType;
 public:
  long long n;
  int d;

  Raw_data(long long n, int d, char * data_file_path) {
    this->n = n;
    this->d = d;
    this->data = new T[n * d];
    FILE *fp = fopen(data_file_path, "rb");
    fread(data, sizeof(T), n * d, fp);
    fclose(fp);
  }

  virtual ~Raw_data() {
    delete[] this->data;
  }

  ResultType cal_squared_dist(long long id, T * q) {
    ResultType res = 0, diff0, diff1, diff2, diff3;
    T * a = &data[d * id];
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

 private:
  T * data;
};

#endif /* RAWDATA_H_ */
