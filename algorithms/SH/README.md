# SH - Selective Hashing. 

[Originial source code](http://www.comp.nus.edu.sg/~dsh/download.html). 

## Prerequisites

- A linux system 
- [A sample dataset](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including its data points, query points and ground truth results. All data files downloaded should be put in the `./knn_benchmark/data` directory.

## Compile (SH/src)

To compile AGH, go to AGH/src and run 
```
build.sh
```

to get the executable file SH.

Note that to handle other dataset, we need to modify the dimensionality and the datasize in constant.h 
```
const int D = 192;
const int datasize = 53387;
```
we also need to change the file name in main.cpp

```
char data_name[20]="audio";
```

## Index and Search (SH/script)

To build the index of SH and conduct the search, enter into folder and run 
```
run.sh
```

to obtain the searching performance of SH. We use 20 values for the array MaxChecked in the main.cpp (e.g. the number of the visited points) to  achieve the trade-off between search speed and search quality (recall). The result are reported in SH/result directory.


