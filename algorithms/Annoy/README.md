# Annoy 

Annoy (Approximate Nearest Neighbors Oh Yeah) is a C++ library with Python
bindings to search for approximate nearest neighbor search with L2 or angular
distance. The version here is based on Annoy's source code obtained on 5
April 2016. Please refer to
[**Annoy** at GitHub](https://github.com/spotify/annoy) for more details.



## Prerequisites

- linux system 
- A
  [sample data ](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data)
  (e.g., audio) has been downloaded, which includes its data points, query
  points, and ground truth results. Note that we need to change data format
  later. All data files downloaded should be put in the `./knn_benchmark/data`
  directory.

 
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


