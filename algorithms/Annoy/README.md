# DPG

Diversified Proximity Graph (**DPG**) is an extension of [**KGraph**](http://www.kgraph.org/) which consider the diversity of the K-NN graph 

## Prerequisites
- A modern compiler that supports C++11 and G++ 4.7
- linux system 
- Boost (dev version) installed. Note that we need to set the **paths** of the Boost installed in **Makefile**.
- [Sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including its data points, query points and ground truth results. Note that we need to change data format later. All data files downloaded should be put the knn_benchmark/data directory.

 
## Compile (DPG/src)

build the K-NN graph construction (the same as KGraph), graph diversification ( result in new DPG index ) and search programs, namely DPG_index, DPG_diverse and DPG_search.

```
build.sh
```
Two data convert programs, fvec2lshkit and lshkit2fvec, are also generated, which are copied to the data directory of the NNS benchmark (i.e, nns_benchmark/data) 


## Construct Diversified Proxmity Graph (DPG) (DPG/script)

```
build_index.sh
```
Build K-NN graphs based on the data points, then diversify the K-NN graph to generate the DPG graph. 
To save the space, one may directly build a DPG graph by diversifying the K-NN graph constructed by KGraph (i.e., index files uder KGraph/index directory). Note that for a DPG graph with L=20 (i.e., 20 neighbors per data point), we need diversify the KGraph with L=40 (i.e., 40 neighbours per data point) because we choose half of the neighbors, and  the also include reverse edges. By doing this, the size of DPG is the same as KGraph in the worse case.   

## Search on the K-NN graph (DPG/script)

```
search.sh
```
Note that, in our experiment paper, we use P value of DPG search (i.e., search queue size) to achieve the trade-off between search speed and search quality (recall). This is exactly the same with KGraph.


