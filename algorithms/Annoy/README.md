# Annoy 

Annoy (Approximate Nearest Neighbors Oh Yeah) is a C++ library with Python bindings to search for points in space that are close to a given query point. Please refer to [**Annoy**](https://github.com/spotify/annoy) for more details. 



## Prerequisites

- linux system 
- [Sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including its data points, query points and ground truth results. Note that we need to change data format later. All data files downloaded should be put the knn_benchmark/data directory.

 
## Compile (Annoy/src)

build the Annoy trees construction and Annoy search programs.

```
make
```


## Construct Annoy trees and conduct search (DPG/script)

```
run.sh
```
Note that, in our experiment paper, we vary the number of data points explored to achieve the trade-off between search speed and search quality (recall). This is exactly the same with KGraph.


