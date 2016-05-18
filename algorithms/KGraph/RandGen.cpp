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

#include <math.h>
#include <stdlib.h>
#include "RandGen.h"

/************************************************************
 ***  Given a mean and a standard deviation, gaussian       **
 **   generates a normally distributed random number        **
 **   Algorithm:  Polar Method, p.  104, Knuth, vol. 2      **
 ************************************************************/

float gaussian(float mean, float sigma) {
  float v1, v2;
  float s;
  float x;

  do {
    v1 = 2 * uniform(0, 1) - 1;
    v2 = 2 * uniform(0, 1) - 1;
    s = v1 * v1 + v2 * v2;
  } while (s >= 1.);

  x = v1 * (float) sqrt(-2. * log(s) / s);

  /*  x is normally distributed with mean 0 and sigma 1.  */
  x = x * sigma + mean;

  return (x);
}

/************************************************************
 ** Generates a random number between _min and _max         **
 ** uniformly                                               **
 ** By Yufei Tao
 ************************************************************/

float uniform(float _min, float _max) {
  int int_r = rand();
  long base = RAND_MAX - 1;
  float f_r = ((float) int_r) / base;
  return (_max - _min) * f_r + _min;
}

