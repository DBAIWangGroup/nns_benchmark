# KGraph

This is a modified version based on http://www.kgraph.org/

## Prerequisites
- A modern compiler that supports C++11 and G++ 4.7
- linux system 
- Boost (dev version) installed. Note that we need to set the paths of the Boost installed in **build.sh**.
 
## Compile 

build the index construction and search programs, namely kgraph_index and kgraph_search.

```
build.sh
```
two data convert programs, fvec2lshkit and lshkit2fvec, are also generated.

In the build.sh, we also copy these two programs to the data directory of the NNS benchmark and 
convert the sample data (audio by default) from [*fvecs*](http://corpus-texmex.irisa.fr/) format to [*lshkit*](http://www.kgraph.org/index.php?n=Main.LshkitFormat) format, including data points, query points and ground truth results.

## Construct K-NN graph 

```
kgraph_index.sh
```

## Search on the K-NN graph

```
kgraph_search.sh
```
Note that, in our experiment paper, we use P value of KGraph search (i.e., search queue size) to achieve the trade-off between search speed and search quality (recall).


