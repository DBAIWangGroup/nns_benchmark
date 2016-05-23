# AGH - Anchor-based Graph Hashing

This is a modified version of AGH. The original version can be downloaded from [here](http://www.ee.columbia.edu/ln/dvmm/downloads/WeiGraphConstructCode2011/dlform.htm), which is implemented with MATLAB. 

We use the original code to generate the hashcodes of the data points and transformation parameters. We rewrite the searching part in C++ to transform the query points to binary vectors and use the hierarchical clustering trees in FLANN (C++) to index the data binary vectors to support more efficient search.

## Compile (AGH/src)

To compile AGH, go to AGH/src and run 

```
make
```

to get the executable file.

## Index (AGH/preprocess)

To build the index of AGH, enter into preprocess folder and run 
```
main.m
```
in MATLAB to get the transformed data and transformation parameters.
Alternatively, one may run 
```
matlab -nodisplay -nojvm -nosplash -nodesktop -r "try, run('main.m'), end, exit(0)"
``` 

Note that main.m will build the index for 1-AGH and 2-AGH. One could choose to generate only one of them.

## Search (AGH/script)

go to AGH/script and run 
```
searcher.sh 
```
to obtain the searching performance of AGH. We use c value (e.g. the number of the visited points) to  achieve the trade-off between search speed and search quality (recall).


