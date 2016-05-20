NonMetricSpaceLib
======================================================

This is a modified version of NonMetricSpaceLib, which contains the implementation of NAPP, SW and VP-Tree. The original version can be found form  [here](https://github.com/searchivarius/nmslib).

Main differences:

We disabled SIMD and multi-threading techniques. We also replace the compiler option -Ofast to -O3.

##Prerequisites:

- A modern compiler that supports C++11: G++ 4.7, Intel compiler 14, Clang 3.4.
- Boost (dev version).
- CMake (version 2.8 or over is needed).
- Eigen (dev version version 3.0 or over is needed).
- [Sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including data points, query point. Note that the input format is .txt and the ground truth results can be generated before evaluating.

##Compile (NonMetricSpaceLib/code) 

To compile, go to the directory code and type:
```
cmake .
```
then
```
make  
```
##Build and Search (NonMetricSpaceLib/script)
Three algorithms are evaluated in our NNS benchmark, including **NAPP**, **SW** and **VP-Tree**.
type run_NAPP.sh, run_SW.sh and run_vptree.sh to get the index and search results of NAAPP, Small World and VP-Tree. 
```
run_NAPP.sh
run_SW.sh
run_vptree.sh 
```

The search performance (time and recall) results are kept in the NonMetricSpaceLib/results directory.

