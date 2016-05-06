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

#ifndef PARAMFILE_H_
#define PARAMFILE_H_

void writeParamFile(char * file_path, long long n, int d, int m, int B,
                    float * proj, const char * type);  // write the para file out (as part of the index)
float * readParamFile(char * index_path, long long &n, int &d, int &m, int &B,
                      char * type);  // read parameters from para file

#endif /* PARAMFILE_H_ */
