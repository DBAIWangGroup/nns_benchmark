# Optimized Product Quantization

This is a combination of source codes of OPQ and Inverted MultiIndex. In our impementation, we use the inverted multi-indexing technique to perform non-exhaustive search for OPQ. [The original code of OPQ](http://kaiminghe.com/cvpr13/index.html) is implemented by MATLAB, which is used to transform the data vectors and query vectors for finding the minimal quantization distortion. 
For the search efficiency,  we apply the [inverted multi-indexing technique](http://arbabenko.github.io/MultiIndex/index.html) implemented by C++ to perform non-exhaustive search. 

##Prerequisites

- `Yael` and `VLFeat` libraries (for the Matlab code)
- A modern compiler that supports `C++11` (e.g., `gcc 4.7` and above)
- Matlab
- CMake (version 2.6 or over)
- The `boost` library (dev version) installed.
- IntelMKL

##Compile (OPQ/src)

To compile, go to src directory and type 

```
./build_release.sh
```

Note that the local directory of `Boost` and `IntelMKL` should be provided (`CMakeLists.txt`).

##Index (OPQ/Transform_Data and OPQ/script)

To get OPQ indexing, one should first enter the `Transform_Data` folder and run 

```
main.m 
```
in the Matlab to get the transformed data vectors and query vectors, coarse vocabularies and fine vocabularies.
Note that the directories of `Yael` and `vlfeat` libraries should be given in the main.m file. 

Then go to OPQ/script directory and run  

```
./indexer.sh 
```

to build the OPQ index.

##Search (OPQ/script) 

```
./search.sh
```

Note that, in our experiment paper, we tune the `c` value of OPQ search (i.e.,
size of neighbors to be visited) to achieve the trade-off between search speed
and search quality (recall). The search performance (time and recall) results
are kept in the `OPQ/results` directory. 


