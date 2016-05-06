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

#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <boost/math/distributions/chi_squared.hpp>

void usage();
void cal_para_given_t(int, double, double, double);
void cal_para_given_m(int, int, double, double);

int main(int argc, char* argv[]) {
  const struct option longopts[] = {
      { "help", no_argument, 0, 'h' },
      { "page-size", required_argument, 0, 'b' },
      { "m", required_argument, 0, 'm' },
      { "cardinality", required_argument, 0, 'n' },
      { "approximation-ratio", required_argument, 0, 'c' },
      { "max-number-of-points", required_argument, 0, 't' },
      { "success-probability", required_argument, 0, 'p' },
      { 0, 0, 0, 0 },
  };

  int index;
  int iarg = 0;
  opterr = 1;    //getopt error message (off: 0)

  double c = 4.0;  //default value
  double success_prob = 0.132121;  // default value: 1/2 - 1/e
  double max_f = 0;
  int m = -1;
  int n = -1;

  bool is_valid_command = true;

  while (iarg != -1) {
    iarg = getopt_long(argc, argv, "m:n:c:t:p:h", longopts, &index);

    switch (iarg) {
      case 'c':
        if (optarg) {
          c = atof(optarg);
        }
        break;
      case 'h':
        usage();
        return 0;
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
          success_prob = atof(optarg);
        }
        break;
      case 't':
        if (optarg) {
          max_f = atof(optarg);
        }
        break;
    }
  }

  if (n <= 0 || success_prob < 0 || success_prob > 1 || c < 1 || max_f < 0
      || max_f > 1 || m < 0 || (max_f != 0 && m != -1)
      || (max_f == 0 && m == -1)) {
    is_valid_command = false;
  } else if (m == -1) {
    cal_para_given_t(n, max_f, c, success_prob);
  } else {
    cal_para_given_m(n, m, c, success_prob);
  }

  if (!is_valid_command) {
    usage();
  }

  return 0;
}

/*
 * Given: c, T/n, n, p_\tau
 * Output: minimum m, p_\tau', and T'
 * Algorithm 6 in the paper
 * */
void cal_para_given_t(int n, double max_f, double c, double sp) {
  int m = 0;
  bool find = false;
  while (!find) {
    m++;
    boost::math::chi_squared chi(m);
    double prev_t = 1.0;
    // check if current m valid
    for (double p = 0.00001; p < 1; p = p + 0.00001) {
      double t = boost::math::cdf(chi, boost::math::quantile(chi, p) / c / c)
          / (p - sp);  //quantile equals to inverse_cdf
      if (t > 0 && t < 1) {
        if (t < max_f) {
          find = true;
          break;
        }
        if (t > prev_t) {
          break;
        }
        prev_t = t;
      }
    }
  }
  if (m > 30) {  // if m is too large, warn user
    printf(
        "Warning: m is too large, please consider enlarge c or t, reduce p\n");
  }
  printf("A feasible setting is:\nm = %d\n", m);

  double prev_t = 1.0;
  boost::math::chi_squared chi(m);
  // find the best threshold for a given m (to minimize T)
  for (double p = 0.000001; p < 1; p += 0.000001) {
    double t = boost::math::cdf(chi, boost::math::quantile(chi, p) / c / c)
        / (p - sp);
    if (t > 0 && t < 1) {
      if (t > prev_t) {
        printf("prob_thres(-r) = %f\nT_max(-t) = %d\nt = %f\n", p, (int) (n * t) + 1,
               t);
        break;
      }
      prev_t = t;
    }
  }
}

/*
 * Given: c, m, n, p_\tau
 * Output: p_\tau' and T'
 * Different from Algorithm 6.
 */
void cal_para_given_m(int n, int m, double c, double sp) {
  double t = 0, p = 0;
  bool find = false;

  boost::math::chi_squared chi(m);
  double prev_t = 1.0;

  // find the best threshold for a given m (to minimize T)
  for (p = 0.000001; p < 1; p += 0.000001) {
    t = boost::math::cdf(chi, boost::math::quantile(chi, p) / c / c) / (p - sp);
    if (t > 0 && t < 1) {
      if (t > prev_t && t < 1) {
        find = true;
        break;
      }
      prev_t = t;
    }
  }

  if (!find) {  // it is possible that given m doesn't work
    printf(
        "Error: Cannot find valid setting. Please consider enlarge c or m, or reduce p\n");
    return;
  }

  printf("A feasible setting is:\nm = %d\n", m);
  printf("prob_thres(-r) = %f\nT_max(-t) = %d\nt = %f\n", p, (int) (n * t) + 1, t);
}

void usage() {
  printf("SRS-Mem (v1.0)\n");
  printf("Options\n");
  printf("-c {value}\tapproximation ratio (>= 1), default value: 4\n");
  printf("-p {value}\tsuccess probability, default value: 1/2 - 1/e\n");
  printf("-m {value}\tdimensionality of the projected space\n");
  printf("-n {value}\tcardinality\n");
  printf("-t {value}\tmaximum fraction of verify points\n");
  printf("\n");

  printf("Usage:\n");
  printf("Calculate a feasible setting of parameters (given m)\n");
  printf("[-c] -m -n [-p]\n");
  printf("Calculate a feasible setting of parameters (given t)\n");
  printf("[-c] -n -p [-t]\n");
}
