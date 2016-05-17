# NNS_Benchmark for Nearest Neighbor Search on High Dimensional Data

Nearest neighbor search (NNS) is fundamental and essential operation in applications from many domains, such as databases, machine learning, multimedia, and computer vision. Over hundreds of algorithms have been proposed to solve the problem
from different perspectives, and this line of research remains very active in the above domains due to its importance and the huge challenges. 

This motivates us to setup a benchmark for Nearest Neighbor Search (NNS) on High Dimensional Data such that users can conduct comprehensive performance evaluation and analysis of the NNS algorithms. 
We believe such a NNS benchmark will be beneficial to both the scientific community and practitioners.

As the first step of the benchmark, we restrict the scope of the study by imposing the following constraints. 

- **Representative and competitive approximate NNS algorithms**. It has been well recognized that the exact NNS algorithms often cannot even outperform the simple linear scan algorithm when the dimensionality is high. We therefore only consider the approximate solutions. In this benchmark, we consider the state-of-the-art algorithms in several domains.

- **No hardware specific optimizations**.  Not all the implementations we obtained or implemented have the same level of
sophistication in utilizing the hardware specific features to speed up the query processing. Therefore, we modified several implementations so that no algorithm uses multiple threads, multiple CPUs, SIMD instructions, hardware pre-fetching, or GPUs.

- **Dense vectors**. We mainly focus on the case where the input data vectors are dense, i.e., non-zero in most of the dimensions.

- **Support the Euclidian distance**. The Euclidean distance is one of the most widely used measures on high-dimensional datasets. It is also supported by most of the NNS algorithms.

- **Exact KNN as the ground truth**. In some existing works, each data point has a label (typically in classification or clustering applications) and the labels are regarded as the ground truth when evaluating the recall of approximate NN algorithms. In our benchmark, we use the exact KNN points as the ground truth as this works for all datasets and majority of the applications.

Currently, we evaluate 15 representative NNS algorithms on 20 datasets where details are reported in our [experiment paper]
(http://www.cse.unsw.edu.au/~yingz/NNS.pdf)[1].

## ALGORITHMS EVALUTED 

All the original source codes used in this benchmark are publicly available. Algorithms are implemented in C++ unless otherwise specified. We carefully go through all the implementations and make necessary modifications to for fair comparisons. For instance, we re-implement the search process of some algorithms in C++. We also disable the multi-threads, SIMD instructions, fast-math, and hardware prefetching technique. 

Below are brief introuductions of the algorithms evaluated in the benchmark as well as our revisions,
which are grouped into four categories.

#### 1. LSH-BASED METHODS
- **QALSH** Query-Aware LSH ([2], PVLDB’15). 
[Originial source code](http://ss.sysu.edu.cn/~fjl/qalsh/qalsh_1.1.2.tar.gz)

- **SRS** ([3], PVLDB’14).
[Originial source code](https://github.com/DBWangGroupUNSW/SRS). 
Note that both external memory and in-memory versions of **SRS** are available.

#### 2. ENCODING-BASED METHODS

- **SGH** Scalable Graph Hashing ([4], IJCAI’15). 
[Originial source code](http://cs.nju.edu.cn/lwj). 
  1. Source codes (index construction and search) are implemented by MATLAB. 
  2. In our implementation, we use the hierarchical clustering trees in **FLANN** (C++) to index the resulting binary vectors to support more efficient search.

- **AGH** Anchor Graph Hashing ([5], ICML’11). 
[Originial source code](http://www.ee.columbia.edu/ln/dvmm/downloads)
  1. Source codes (index construction and search) are implemented by MATLAB. 
  2. In our implementation, we use the hierarchical clustering trees in **FLANN** (C++) to index the resulting binary vectors to support more efficient search.


- **NSG** Neighbor-Sensitive Hashing ([6], PVLDB’15). 
[Originial source code](https://github.com/pyongjoo/nsh)
  1. Source codes (index construction and search) are implemented by MATLAB. 
  2. In our implementation, we use the hierarchical clustering trees in **FLANN** (C++) to index the resulting binary vectors to support more efficient search.

- **SH** Selective Hashing ([7], KDD’15). 
[Originial source code](http://www.comp.nus.edu.sg/~dsh/download.html). Note that we have confirmed with the author that the source code released is for KDD'15 paper, not their previous SIGMOD'14 paper.  

- **OPQ** Optimal Product Quantization ([8], TPAMI’14). 
[Originial source code](http://research.microsoft.com/en-us/um/people/kahe). 
Note that, in our impementation, we use the [inverted multi-indexing technique](http://arbabenko.github.io/MultiIndex/index.html) [17] to perform non-exhaustive search for **OPQ**.

- **NAPP** Neighborhood APProximation index ([9], PVLDB’15). 
[Originial source code](https://github.com/searchivarius/nmslib)
  1. We disable the SIMD, multi-threading, and -Ofast compiler option.

#### 3. TREE-BASED SPACE PARTITION METHODS

- **FLANN** ([10], TPAMI’14). 
[Originial source code](http://www.cs.ubc.ca/research/flann)
  1.  We disable the multi-threading techniques in **FLANN**.

- **Annoy** ([11]). 
[Originial source code](https://github.com/spotify/annoy)
  1. Source codes are implemented by C++ (core algorithms) and Python (for binding). We re-implemente the Python binding part with C++. 
  2. We disable -ffast-math compiler option in **Annoy**.

- **VP-Tree** Vantage-Point tree ([12], NIPS’13). 
[Originial source code](https://github.com/searchivarius/nmslib)
  1. We disable the SIMD, multi-threading, and -Ofast compiler option.

#### 4. NEIGHBORHOOD-BASED METHODS

- **SW** Small World Graph ([13], IS'14). 
[Originial source code](https://github.com/searchivarius/nmslib)
  1. We disable the SIMD, multi-threading, and -Ofast compiler option.

- **RCT** Rank Cover Tree ([14], TPAMI'15).
Originial source code is obtained from authors by email, who kindly allow us to release them thorough this benchmark. 

- **KGraph** ([15] [16], WWW'11). 
[Originial source code](https://github.com/aaalgo/kgraph)
  1. We disabled SIMD and multi-threading techniques in **KGraph**.

- **DPG** Diversified Proximity Graph ([1]). 
[Originial source code](https://github.com/DBWangGroupUNSW/nns_benchmark)


## DATASETS USED 

We deploy **18 real datasets** used by existing works which cover a wide range of applications including image, audio,
vedio and textual data. We also use **2 sythetic datasets**. Table 1 summarizes the characteristics of the datasets including
the number of data points (n), dimensionality (d), Relative Contrast (RC [18]), local intrinsic dimensionality (LID [19]), and data type where RC and LID are used to describe the hardness of the datasets.

| **Name** | **n X 10^3** | **d** | **RC** | **LID** | **Type**|
| ------ | ------ | ------ | ------ | ------ | -------- |
| Nus*   | 269    |  500    |  1.67 |  24.5 | Image |
| Gist*  | 983    |  960    |  1.94   | 18.9  | Image |
| Rand*  | 1,000  |  100    | 3.05    | 58.7  | Synthetic |
| Glove* | 1,192  |  100    | 1.82    | 20.0  | Text  | 
| Cifa   | 50     |  512    | 1.97    | 9.0   | Image |
| Audio  | 53     |  192    | 2.97    | 5.6   | Audio |
| Mnist  | 69     |  784    | 2.38    | 6.5   | Image |
| Sun    | 79     |  512    | 1.94    | 9.9   | Image |
| Enron  | 95     |  1,369  | 6.39    | 11.7  | Text  |
| Trevi  | 100    |  4,096  | 2.95    | 9.2   | Image |
| Notre  | 333    |  128    | 3.22    | 9.0   | Image |
| Yout   | 346    |  1,770  | 2.29    | 12.6  | Video |
| Msong  | 922    |  420    | 3.81    | 9.5   | Audio |
| Sift   | 994    |  128    | 3.50    | 9.3   | Image |
| Deep   | 1,000  |  128    | 1.96    | 12.1  | Image |
| Ben    | 1,098  |  128    | 1.96    | 8.3   | Image |
| Imag   | 2,340  |  150    | 2.54    | 11.6  | Image |
| Gauss  | 2,000  |  512    | 3.36    | 19.6  | Synthetic   |
| UQ-V   | 3,038  |  256    | 8.39    | 7.2   | Video |
| BANN   | 10,000 |  128    | 2.60    | 10.3  | Image |

**Table 1: Dataset Summary**

We mark the first four datasets in Table 1 with asterisks to indicate that they are “hard” datasets compared with
others according to their RC and LID values.
Below, we describe the datasets used in the experiments.

- [**Sift**](http://corpus-texmex.irisa.fr) consists of 1 million 128-d SIFT vectors.

- [**Nusw**](http://lms.comp.nus.edu.sg/research/NUS-WIDE.htm) includes around $2.7$ million web images,
each as a 500-dimensional bag-of-words vector.

- [**Msong**](http://www.ifs.tuwien.ac.at/mir/msd/download.html) is a collection of audio features and metadata for a million contemporary popular music tracks with $420$ dimensions.

- [**Gist**]() is is an image dataset which contains about 1 million data points with 960 dimensions.

- [**Deep**](https://yadi.sk/d/I_yaFVqchJmoc) contains deep neural codes of natural images obtained from the activations of a convolutional neural network, which contains about 1 million data points with 256 dimensions.

- [**Bann**]() is used to evaluate the scalability of the algorithms, where 1M, 2M, 4M, 6M, 8M, and 10M data points are sampled from 128-dimensional SIFT descriptors extracted from natural images.

- **Guass** is generated by randomly choosing $1000$ cluster centers with in space [0,10]^512, and each cluster follows the a Gaussian distribution with deviation 1 on each dimension.

- **Random** contains $1$M randomly chosen points in a unit hypersphere with dimensionality 100.

- [**Audio**](http://www.cs.princeton.edu/cass/audio.tar.gz) has about 0.05 million 192-d audio feature vectors extracted by Marsyas library from DARPA TIMIT audio speed dataset.

- [**Cifar**](http://www.cs.toronto.edu/~kriz/cifar.html) is a labeled subset of *TinyImage* dataset, which consists of 60000 32 X color images in 10 classes, with each image represented by a 512-d GIST feature vector.

- [**Enron**]() origins from a collection of emails. Yifang et. al. extract bi-grams and form feature vectors of 1369 dimensions. 

- [**Glove**](http://nlp.stanford.edu/projects/glove) contains 1.2 million 100-d word feature vectors extracted from Tweets.

- [**Imag**](http://cloudcv.org/objdetect) is introduced and employed by ``The ImageNet Large Scale Visual Recognition Challenge(ILSVRC)'', which contains about 2.4 million data points with 150 dimensions dense SIFT features.

- [**Mnist**](http://yann.lecun.com/exdb/mnist) consists of 70k images of hand-written digits, each as a 784-d vector concatenating all pixels. we randomly sample 1k as the queries and use the remaining as the data base.
 
- [**Sun**](http://groups.csail.mit.edu/vision/SUN) contains about 0.08 million 512-d GIST features of images.

- [**Notre**](http://phototour.cs.washington.edu/datasets) contains about 0.3 million 128-d features of a set of Flickr images and a reconstruction.

- [**UKbench**](http://vis.uky.edu/~stewe/ukbench) contains about 1 million 128-d features of images.

- [**Trevi**](http://phototour.cs.washington.edu/patches/default.htm) consists of 0.4 million X 1024 bitmap(.bmp) images, each containing a 16 X 16 array of image patches. Each patch is sampled as 64 X 64 grayscale, with a canonical scale and orientation. Therefore, Trevi patch dataset consists of around 100,000 4096-d vectors.

- [**UQ_V**]() is a video dataset. a local feature based on some keyframes are extracted which include 256 dimensions.

- [**Yout**](http://www.cs.tau.ac.il/~wolf/ytfaces/index.html) contains 3,425 videos of 1,595 different people. All the videos were downloaded from YouTube. An average of 2.15 videos are available for each subject. The shortest clip duration is 48 frames, the longest clip is 6,070 frames, and the average length of a video clip is 181.3 frames.

## PERFORMANCE EVALUATION and ANALYSES 

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

[18] J. He, S. Kumar, and S. Chang. *On the difficulty of nearest neighbor search*. In ICML, 2012.

[19] L. Amsaleg, O. Chelly, T. Furon, S. Girard, M. E. Houle, K. Kawarabayashi, and M. Nett. *Estimating local intrinsic dimensionality*. In SIGKDD, 2015.








