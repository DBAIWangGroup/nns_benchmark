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

#ifndef PROJDATA_H_
#define PROJDATA_H_

// Similar to raw_data, but for projected data, as raw data are 
// integers while projected data are floating numbers
class Proj_data {
 private:
  long long n;
  int d;

 public:
  float * data;  // data
  Proj_data(long long n, int d, float *data);
  virtual ~Proj_data();

  float cal_squared_dist(long long id1, long long id2);  // calculate distance between two points in the dataset (given their ids)
  float cal_squared_dist(long long id, float *q);  // calculate distance between given vector (i.e., q) and point in the dataset (i.e., id)
  float cal_dist(long long id1, long long id2);  // calculate distance between two points in the dataset (given their ids)
  float cal_dist(long long id, float *q);  // calculate distance between given vector (i.e., q) and point in the dataset (i.e., id)

  void print_row(long long i);  // print out the i-th vector in the dataset (i start from 0)
};

#endif /* PROJDATA_H_ */
