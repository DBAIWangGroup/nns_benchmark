# KGraph

This is a modified version based on http://www.kgraph.org/

## Prerequisites
- A modern compiler that supports C++11 and G++ 4.7
- linux system 
- Boost (dev version) installed. Note that we need to set the **paths** of the Boost installed in **Makefile**.
- [Sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including its data points, query points and ground truth results. Note that we need to change data format later. All data files downloaded should be put the knn_benchmark/data directory.
 
## Compile 

build the index construction and search programs, namely kgraph_index and kgraph_search.

```
build.sh
```
Two data convert programs, fvec2lshkit and lshkit2fvec, are also generated, which are copied to the data directory of the NNS benchmark (i.e, nns_benchmark/data) 

## convert data 

```
convert_data.sh 
```

Convert the sample data (audio by default) from [*fvecs*](http://corpus-texmex.irisa.fr/) format to [*lshkit*](http://www.kgraph.org/index.php?n=Main.LshkitFormat) format, including data points, query points and ground truth results. Please ensure that the data with fvecs format have been downloaded and put to the data directory (i.e, nns_benchmark/data). 

## Construct K-NN graph 

```
kgraph_index.sh
```
Build K-NN graphs based on the data points, which is put to its index directory (i.e., nns_benchmark/index/kgraph).
Note that the suffix of the index file name (e.g., audio.kgraph_40) represents the number of neighbors kept in the K-NN graph.

## Search on the K-NN graph

```
kgraph_search.sh
```
Note that, in our experiment paper, we use P value of KGraph search (i.e., search queue size) to achieve the trade-off between search speed and search quality (recall).


