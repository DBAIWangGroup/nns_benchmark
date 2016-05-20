# RCT - Rank Cover Tree

Originial source code is obtained from authors by email, who kindly allow us to release them through this benchmark.  

## Prerequisites
- A modern compiler that supports G++ 4.7
- [Sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including data points, query points and ground truth results. Note that all data files downloaded should be put to the knn_benchmark/data directory.
 
## Compile  (RCT/src)

To compile the RCT program, go to src and type

make

## Construct (RCT/script) 

```
build.sh
```
Build index based on the data points, which is put to its index directory (i.e., RCT/index).

## Search (RCT/script)
```
search.sh
```
Note that, in our experiment paper, we use c value of RCT search(i.e. restricting the number of neighboring nodes to be visited at each level of the structure) to achieve the trade-off between search speed and search quality (recall). The search performance (time and recall) results are kept in the RCT/results directory.

