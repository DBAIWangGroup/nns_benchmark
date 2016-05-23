# NSH - Neighbor-Sensitive Hashing

This is a modified version of NSH. The original version can be downloaded from [here](https://github.com/pyongjoo/nsh), which is implemented with MATLAB. 

We use the original code to generate the hashcodes of the data points and transformation parameters. We rewrite the searching part in C++ to transform the query points to binrary vectors and use the hierarchical clustering trees in FLANN (C++) to index the data binary vectors to support more efficient search.

## Compile (NSH/src)

To compile NSH, go to NSH/src and run 
```
make
```
to get the executable file `NSH`.

## Index (NSH/preprocess)

To build the index of NSH, enter into preprocess folder and run 
```
main.m
```
in MATLAB to get the transformed data and transformation parameters.
Alternatively, one may run
```
matlab -nodisplay -nojvm -nosplash -nodesktop -r "try, run('main.m'), end, exit(0)"
```

## Search (NSH/script)

go to NSH/src and run 
```
searcher.sh 
```
to obtain the searching performance of NSH. We use c value (e.g. the number of the visited points) to  achieve the trade-off between search speed and search quality (recall).


