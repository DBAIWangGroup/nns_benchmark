SRS - Fast Approximate Nearest Neighbor Search in High Dimensional Euclidean Space With a Tiny Index
====================================================================================================

Please refer to our [GitHub reposit](https://github.com/DBWangGroupUNSW/SRS) for more details.

Prerequisites.
------------------

- A modern compiler that supports `C++11` (e.g., `gcc 4.7` and above)
- A linux system 
- The users need to have Boost C++ library installed (http://www.boost.org/). The Boost library is used to calculate the quantile of chi-squared distribution.
- [A sample data](https://github.com/DBWangGroupUNSW/nns_benchmark/tree/master/data) (e.g., audio) is downloaded, including its data points, query points and ground truth results. Note that we need to change data format later. All data files downloaded should be put the `./knn_benchmark/data` directory.


How to use SRS
--------------

1. Compile the program (SRS/src)

```
% make all
```

2. Build Index (SRS/script)

```
% build.sh
```
3. Conduct search (SRS/script)

```
% search.sh
```


Data Format
-----------

1. Data file should contain _n_ lines, where _n_ is the cardinality of the
   dataset. The file should be formatted as:

```
e_1_1 e_1_2 ... e_1_d
...
e_n_1 e_n_2 ... e_n_d
```
  
  where `e_i_j`s are integers, and are separated by whitespace.

2. Query file should contain _N+1_ lines, where _N_ is the number of queries in the
   query workload. The file should be formatted as:
  
```
N d
ID_1 e_1_1 e_1_2 ... e_1_d
...
ID_N e_N_1 e_N_2 ... e_N_d
```
  where _d_ is the dimensionality, `e_i_j` is an integer, and separated by whitespace.


