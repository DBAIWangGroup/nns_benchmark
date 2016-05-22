# KGraph

This is a modified version of [**KGraph**](http://www.kgraph.org) from the
KGraph source code obtained on 5 April 2016. The original version of **KGraph** which is continuously maintained by [Dr. Wei Dong](https://github.com/aaalgo).

Main differences:

  1. We disabled SIMD and multi-threading techniques in **KGraph**. Note the besides the compiler flag, we also comment the SIMD related code in `metric.cpp`
  
  2. We reduce the index size of KGraph by not keeping the distances of the edges, which is not used in the search process. 
  

## Prerequisites
- A modern compiler that supports `C++11` (e.g., `g++ 4.7` and above)
- A Linux System 
- The boost library (dev version) installed. Note that we need to set the **paths** of the Boost installed in **Makefile**.
- [A sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including data points, query points and ground truth results. Note that all data files downloaded should be put to the `./knn_benchmark/data` directory.
 
## Compile  (KGraph/src)

build the index construction and search programs, namely `kgraph_index` and `kgraph_search`.

```
build.sh
```

## Construct K-NN graph (KGraph/script) 

``` kgraph_index.sh ```

Build K-NN graphs based on the data points, which is put
to its index directory (i.e., `./nns_benchmark/index/kgraph`). Note that the
suffix number of the index file (e.g., `kgraph_40` of `audio.kgraph_40`)
represents the number of neighbors kept in the K-NN graph.

## Search on the K-NN graph (KGraph/script)

```
kgraph_search.sh
```

Note that, in our experiment paper, we use `P` value of KGraph search (i.e.,
search queue size) to achieve the trade-off between search speed and search
quality (recall). The search performance (time and recall) results are kept in
the KGraph/results directory.

