# SGH - Scalable Graph Hashing

This is a modified version of SGH. The original version can be downloaded from [here](http://cs.nju.edu.cn/lwj/), which is implemented with MATLAB. 

We use the original code to generate the hashcodes of the data points and transformation parameters. We rewrite the searching part in C++ to transform the query points to binrary vectors and use the hierarchical clustering trees in FLANN (C++) to index the data binary vectors to support more efficient search.

## Compile (SGH/src)

To compile SGH, go to SGH/src and run 
```
make
```
to get the executable file 'SGH'.

## Index (SGH/preprocess)

To build the index of SGH, enter into preprocess folder and run 
```
main.m
```
in MATLAB to get the transformed data and transformation parameters. Alternatively, one may run
```
matlab -nodisplay -nojvm -nosplash -nodesktop -r "try, run('main.m'), end, exit(0)"
```

## Search (SGH/script) 

go to SGH/script and run 
```
searcher.sh 
```
to obtain the searching performance of SGH. We use c value (e.g. the number of the visited points) to  achieve the trade-off between search speed and search quality (recall).


