# NNS_Benchmark for Nearest Neighbor Search on High Dimensional Data

Nearest neighbor search (NNS) is fundamental and essential operation in applications from many domains, such as databases, machine learning, multimedia, and computer vision. Over hundreds of algorithms have been proposed to solve the problem
from different perspectives, and this line of research remains very active in the above domains due to its importance and the huge challenges. 

This motivates us to setup a benchmark for Nearest Neighbor Search (NNS) on High Dimensional Data such that users can conduct comprehensive performance evaluation and analysis of the NNS algorithms. 
We believe such a NNS benchmark will be beneficial to both the scientific community and practitioners.

As the first step of the benchmark, we restrict the scope of the study by imposing the following constraints. 

- **Representative and competitive NNS algorithms**. It has been well recognized that the exact NNS algorithms often cannot even outperform the simple linear scan algorithm when the dimensionality is high. We therefore only consider the approximate solutions. In this benchmark, we consider the state-of-the-art algorithms in several domains.

- **No hardware specific optimizations**.  Not all the implementations we obtained or implemented have the same level of
sophistication in utilizing the hardware specific features to speed up the query processing. Therefore, we modified several implementations so that no algorithm uses multiple threads, multiple CPUs, SIMD instructions, hardware pre-fetching, or GPUs.

- **Dense vectors**. We mainly focus on the case where the input data vectors are dense, i.e., non-zero in most of the dimensions.

- **Support the Euclidian distance**. The Euclidean distance is one of the most widely used measures on high-dimensional datasets. It is also supported by most of the NNS algorithms.

- **Exact KNN as the ground truth**. In some existing works, each data point has a label (typically in classification or clustering applications) and the labels are regarded as the ground truth when evaluating the recall of approximate NN algorithms. In our benchmark, we use the exact KNN points as the ground truth as this works for all datasets and majority of the applications.

Currently, we evaluate 15 representative NNS algorithms on 20 datasets where details are reported in our [experiment paper]
(http://www.cse.unsw.edu.au/~yingz/NNS.pdf)[1].

## ALGORITHMS EVALUTED 

Below are brief introuductions of the algorithms evaluated in the benchmark, which are grouped into four categories.

#### 1. LSH-BASED METHODS
- **QALSH** Query-Aware LSH ([2], PVLDB’15). 
[Originial source code](http://ss.sysu.edu.cn/~fjl/qalsh/qalsh_1.1.2.tar.gz)

- **SRS** ([3], PVLDB’14).
[Originial source code](https://github.com/DBWangGroupUNSW/SRS)

Note that both external memory and in-memory versions of **SRS** are available.

#### 2. ENCODING-BASED METHODS

- **SGH** Scalable Graph Hashing ([4], IJCAI’15). 
[Originial source code](http://cs.nju.edu.cn/lwj)

- **AGH** Anchor Graph Hashing ([5], ICML’11). 
[Originial source code](http://www.ee.columbia.edu/ln/dvmm/downloads)

- **NSG** Neighbor-Sensitive Hashing ([6], PVLDB’15). 
[Originial source code](https://github.com/pyongjoo/nsh)

- **SH** Selective Hashing ([7], KDD’15). 
[Originial source code](http://www.comp.nus.edu.sg/~dsh/download.html). Note that we have confirmed with the author that the source code is KDD'15 paper, not the SIGMOD'14 paper.  

- **OPQ** Optimal Product Quantization ([8], TPAMI’14). 
[Originial source code](http://research.microsoft.com/en-us/um/people/kahe). 
Note that, in our impementation, we use the [inverted multi-indexing technique](http://arbabenko.github.io/MultiIndex/index.html) [17] to perform non-exhaustive search for **OPQ**.

- **NAPP** Neighborhood APProximation index ([9], PVLDB’15). 
[Originial source code](https://github.com/searchivarius/nmslib)

#### 3. TREE-BASED SPACE PARTITION METHODS

- **FLANN** ([10], TPAMI’14). 
[Originial source code](http://www.cs.ubc.ca/research/flann)

- **Annoy** ([11]). 
[Originial source code](https://github.com/spotify/annoy)

- **VP-Tree** Vantage-Point tree ([12], NIPS’13). 
[Originial source code](https://github.com/searchivarius/nmslib)

#### 4. NEIGHBORHOOD-BASED METHODS

- **SW** Small World Graph ([13], IS'14). 
[Originial source code](https://github.com/searchivarius/nmslib)

- **RCT** Rank Cover Tree ([14], TPAMI'15)
Originial source code is obtained from authors by email, who kindly allow us to release them thorough this benchmark. 

- **KGraph** ([15] [16], WWW'11). 
[Originial source code](https://github.com/aaalgo/kgraph)

- **DPG** Diversified Proximity Graph ([1]). 
[Originial source code](https://github.com/DBWangGroupUNSW/nns_benchmark)

### IMPLEMENTATION ISSUES 

## EXPERIMENT PAPER  

In our [experiment paper](http://www.cse.unsw.edu.au/~yingz/NNS.pdf)[1], we conduct comprehensive experimental study of state-of-the-art NNS methods across several different research areas. Our contribution include:

- comparing all the methods without adding any implementation tricks, which makes the comparison more fair; 
- evaluating all the methods using multiple measures; and
- providing a deeper understanding of the state-of-the-art with new insights into the strength and weakness of each method. We also provide some suggestions about how to select the method under different settings.
- providing rule-of-the-thumb recommendations about how to select the method under different settings.

- We group algorithms into several categories, and then perform detailed analysis on both intra- and inter-category evaluations. Our data-based analyses provide confirmation of useful principles to solve the problem, the strength and weakness of some of the best methods, and some initial explanation and understanding of why some datasets are harder than others. The experience and insights we gained throughout the study enable us to engineer a  new empirical algorithm, DPG, that outperforms all the existing algorithms in majority of the settings and is the most robust in most aspects.

 
## REFERENCES 

[1] W. Li, Y. Zhang, Y. Sun, W. Wang, W. Zhang, X. Lin, *Nearest Neighbor Search on High Dimensional Data — Experiments, Analyses, and Improvement*. [Full Version](http://www.cse.unsw.edu.au/~yingz/NNS.pdf)

[2] Q. Huang, J. Feng, Y. Zhang, Q. Fang, and W. Ng. *Query-aware locality-sensitive hashing for approximate nearest neighbor search*. PVLDB, 9(1):1–12, 2015.

[3] Y. Sun, W. Wang, J. Qin, Y. Zhang, and X. Lin. *SRS: solving c-approximate nearest neighbor queries in high
dimensional euclidean space with a tiny index*. PVLDB,8(1):1–12, 2014

[4] Q. Jiang and W. Li. *Scalable graph hashing with feature transformation*. In IJCAI, pages 2248–2254, 2015.

[5] W. Liu, J. Wang, S. Kumar, and S. Chang. *Hashing with graphs*. In ICML, pages 1–8, 2011.

[6] Y. Park, M. J. Cafarella, and B. Mozafari. *Neighbor-sensitive hashing*. In PVLDB, 9(3):144–155, 2015.

[7] J. Gao, H. V. Jagadish, B. C. Ooi, and S. Wang. *Selective hashing: Closing the gap between radius search and k-nn search*. In SIGKDD, 2015.

[8] T. Ge, K. He, Q. Ke, and J. Sun. *Optimized product quantization*. IEEE Trans. Pattern Anal. Mach. Intell., 36(4):744–755, 2014.

[9] B. Naidan, L. Boytsov, and E. Nyberg. *Permutation search methods are efficient, yet faster search is possible*. PVLDB,
8(12):1618–1629, 2015.

[10] M. Muja and D. G. Lowe. *Scalable nearest neighbor algorithms for high dimensional data*. IEEE Trans. Pattern
Anal. Mach. Intell., 36(11):2227–2240, 2014.

[11] E. Bernhardsson. [*Annoy at github*](https://github.com/spotify/annoy).

[12] L. Boytsov and B. Naidan. *Learning to prune in metric and non-metric spaces*. In NIPS, 2013.

[13] Y. Malkov, A. Ponomarenko, A. Logvinov, and V. Krylov. *Approximate nearest neighbor algorithm based on navigable small world graphs*.  Inf. Syst., 45:61–68, 2014

[14] M. E. Houle and M. Nett. *Rank-based similarity search: Reducing the dimensional dependence*. IEEE TPAMI, 37(1):136–150, 2015.

[15] W. Dong. [*Kgraph website*](http://www.kgraph.org).

[16] W. Dong, M. Charikar, and K. Li. *Efficient k-nearest neighbor graph construction for generic similarity measures*. In WWW, 2011.

[17] A. Babenko and V. S. Lempitsky. *The inverted multi-index*. In CVPR, pages 3069–3076, 2012.




