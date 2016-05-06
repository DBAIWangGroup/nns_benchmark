// -----------------------------------------------------------------------------
// Copyright (c) 2013-2018 Sun Yat-Sen University (SYSU). All Rights Reserved.
//
// SYSU grants permission to use, copy, modify, and distribute this software
// and its documentation for NON-COMMERCIAL purposes and without fee, provided 
// that this copyright notice appears in all copies.
//
// SYSU provides this software "as is," without representations or warranties
// of any kind, either expressed or implied, including but not limited to the
// implied warranties of merchantability, fitness for a particular purpose, 
// and noninfringement. SYSU shall not be liable for any damages arising from
// any use of this software.
//
// Authors: Qiang Huang  (huangq2011@gmail.com)
//          Jianlin Feng (fengjlin@mail.sysu.edu.cn)

//
// Created on:       2014-03-12
// Last Modified on: 2016-03-16
// Version 1.1.2
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <algorithm>
#include <time.h>

// -----------------------------------------------------------------------------
//  For Linux directory
// -----------------------------------------------------------------------------
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

// -----------------------------------------------------------------------------
//  For Windows directory
// -----------------------------------------------------------------------------
//#include <direct.h>
//#include <io.h>


#include "def.h"
#include "util.h"
#include "random.h"
#include "block_file.h"
#include "b_node.h"
#include "b_tree.h"
#include "qalsh.h"
#include "ann.h"

using namespace std;

