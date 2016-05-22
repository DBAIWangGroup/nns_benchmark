# DPG

Diversified Proximity Graph (**DPG**) is an extension of [**KGraph**](http://www.kgraph.org/) which maximizes the diversity of the K-NN graph 

## Prerequisites

- A modern compiler that supports `C++11` (e.g., `gcc 4.7` and above)
- A linux system 
- The `boost` library (dev version) installed. Note that we need to set the **paths** of the Boost installed in **Makefile**.
-
  [A sample dataset](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data)
  (e.g., audio) is downloaded, including its data points, query points and
  ground truth results. Note that we need to change data format later. All data
  files downloaded should be put in the `./knn_benchmark/data` directory.

 
## Compile (DPG/src)

The following script builds three program for K-NN graph construction (the same as KGraph),
performs graph diversification (resulting in the new DPG index); they are
`DPG_index`, `DPG_diverse` and `DPG_search`, respectively. 

```
build.sh
```

Two data conversion programs, `fvec2lshkit` and `lshkit2fvec`, are also
generated, which are copied to the data directory of the NNS benchmark (i.e,
`./nns_benchmark/data`)


## Construct Diversified Proxmity Graph (DPG) (DPG/script)

```
build_index.sh
```

Build K-NN graphs based on the data points, then diversify the K-NN graph to generate the DPG graph. 
To save the space, one may directly build a DPG graph by diversifying the K-NN
graph constructed by KGraph (i.e., index files uder KGraph/index directory).
Note that for a DPG graph with `L=20` (i.e., 20 neighbors per data point), we
need diversify a KGraph index created with `L=40` (i.e., 40 neighbours per data
point) because we only keep half of the neighbors to allow space to include
reverse edges. By doing this, the size of DPG is guaranteed to be the same as
KGraph in the worse case, and is typically smaller.    

## Search on the K-NN graph (DPG/script)

```
search.sh
```

Note that, in our experiment paper, we tune the `P` value of DPG search (i.e.,
search queue size) to achieve the trade-off between search speed and search
quality (recall). This is exactly the same with KGraph.


