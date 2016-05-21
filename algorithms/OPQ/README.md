Optimized Product Quantization

This is a combination scource code of OPQ and Inverted MultiIndex. In our impementation, we use the inverted multi-indexing technique to perform non-exhaustive search for OPQ. The original code of OPQ is implemented by MATLAB, which is used to transform the data vectors and query vectors for finding the minimal quantization distortion. 

##Prerequisites

- Yael and vlfeat library(for Matlab code)
- A modern compiler that supports C++11 and G++ 4.7
- MATLAB
- CMake (version 2.6 or over)
- Boost version (dev version)
- IntelMKL

##Compile

To compile, go to src directory and type 

sh ./build_debug.sh or ./build_release.sh 

Note that the local directory of Boost and IntelMKL should be provided(CMakeLists.txt).

##Index

To get OPQ indexing, one should first enter Transform_Data folder and run 

main.c 

file to get the transformed data vectors and query vectors, coarse vocabularies and fine vocabularies. Note that the directory of Yael and vlfeat library should be given. Then go to OPQ/script directory and run 
 
./indexer.sh 

to obtain the OPQ index.

##Search 

./search.sh 

Note that, in our experiment paper, we use c value of OPQ search (i.e., size of neighbors to be visited) to achieve the trade-off between search speed and search quality (recall). The search performance (time and recall) results are kept in the OPQ/results directory.


