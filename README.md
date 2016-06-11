# NNS Benchmark: Evaluating Approximate Nearest Neighbor Search Algorithms in High Dimensional Euclidean Space

Nearest neighbor search (NNS) is a fundamental and essential operation in applications from many domains, such as databases, machine learning, multimedia, and computer vision. Hundreds of algorithms have been proposed to attack the problem from different perspectives, yet there is no **open** and **comprehensive** comparison. By "comprehensive", we mean using state-of-the-art algorithms proposed from different research areas, and evaluating them on a diverse range of datasets.

To aid researchers and practitioners working on or whose work depends on the problem, we setup this benchmark for Nearest Neighbor Search (NNS) based on the Euclidean distance on High Dimensional Data. The benefit is twofold:

- For researchers, it allows one to **easily** compare their new algorithms with state-of-the-art ones and on a diverse set of datasets. The latter is especially important to gain a **complete** picture of the performance of an algorithm.
- For practitioners, it allows one to **easily** understand the performances of different algorithms and their **tradeoffs**. This helps them to choose the best implementation meeting their own goals and constraints.

We also would like to leverage the entire community to collaboratively build and maintain this benchmark. For example, submitting new algorithms, useful datasets, or offering suggestions or improvements.

## Scope

We limit the scope of this benchmark by imposing the following constraints. 

- **Representative and competitive approximate NNS algorithms**. It has been well recognized that the exact NNS algorithms often cannot even outperform even the simple linear scan algorithm when the dimensionality is high. We therefore only consider the approximate solutions. In this benchmark, we consider the state-of-the-art algorithms in several research areas and those from practitioners.

- **No hardware specific optimizations**.  Not all the implementations we obtained or implemented have the same level of sophistication in utilizing the hardware specific features to speed up the query processing. To perform a (relatively) fair comparison and gain insight into the real strength and weakness of algorithms, we modified several implementations so that, as far as we are aware, no implementation of algorithm in this benchmark uses multiple threads, multiple CPUs, SIMD instructions, hardware pre-fetching, or GPUs.

- **Dense vectors**. We mainly focus on the case where the input data vectors are dense, i.e., non-zero in most of the dimensions.

- **Support the Euclidian distance**. The Euclidean distance is one of the most widely used measures on high-dimensional datasets. It is also supported by most of the NNS algorithms.

- **Exact kNN as the ground truth**. In some existing works, each data point has a label (typically in classification or clustering applications) and the labels are regarded as the ground truth when evaluating the recall of approximate NN algorithms. In our benchmark, we use the exact kNN points as the ground truth as this works for all datasets and majority of the applications.

Currently, we evaluate **15 representative NNS algorithms** on **20 datasets** where details are reported in our [Nearest Neighbor Search (NNS) Experimental Evaluation Paper](http://www.cse.unsw.edu.au/~yingz/NNS.pdf)[1].

## Algorithms Evaluted 

Thanks to the original authors, all the algorithms considered in this benchmark have their sources publicly available. Algorithms are implemented in C++ unless otherwise specified. We carefully go through all the implementations and make necessary modifications to for fair comparisons. For instance, we re-implement the search process of some algorithms in C++. We also disable the multi-threads, SIMD instructions, fast-math, and hardware prefetching technique.

Below are brief introuductions of the algorithms evaluated in the benchmark as well as our revisions, which are grouped into three/four categories.

### 1. Locality Sensitive Hashing (LSH)-Based Methods

- **QALSH** Query-Aware LSH ([2], PVLDB’15). 
[Originial source code](http://ss.sysu.edu.cn/~fjl/qalsh/qalsh_1.1.2.tar.gz)

- **SRS** ([3], PVLDB’14).
[Originial source code](https://github.com/DBWangGroupUNSW/SRS). 
Note that both external memory and in-memory versions of **SRS** are available.

### 2. Space Partitioning-based Methods

#### 2.1. Encoding-Based Space Partitioning Methods

- **SGH** Scalable Graph Hashing ([4], IJCAI’15). 
[Originial source code](http://cs.nju.edu.cn/lwj). 
    * Source codes (index construction and search) are implemented by MATLAB. 
    * In our implementation, we use the hierarchical clustering trees in **FLANN** (C++) to index the resulting binary vectors to support more efficient search.

- **AGH** Anchor Graph Hashing ([5], ICML’11). 
[Originial source code](http://www.ee.columbia.edu/ln/dvmm/downloads)
    * Source codes (index construction and search) are implemented by MATLAB. 
    * In our implementation, we use the hierarchical clustering trees in **FLANN** (C++) to index the resulting binary vectors to support more efficient search.


- **NSG** Neighbor-Sensitive Hashing ([6], PVLDB’15). 
[Originial source code](https://github.com/pyongjoo/nsh)
    * Source codes (index construction and search) are implemented by MATLAB. 
    * In our implementation, we use the hierarchical clustering trees in **FLANN** (C++) to index the resulting binary vectors to support more efficient search.

- **SH** Selective Hashing ([7], KDD’15). 
[Originial source code](http://www.comp.nus.edu.sg/~dsh/download.html).
    Note that we have confirmed with the authors that the source code released implements the algorithm in their KDD'15 paper, not that in their previous SIGMOD'14 paper.  

- **OPQ** Optimal Product Quantization ([8], TPAMI’14). 
[Originial source code](http://research.microsoft.com/en-us/um/people/kahe). 
Note that, in our impementation, we use the [inverted multi-indexing technique](http://arbabenko.github.io/MultiIndex/index.html) [17] to perform non-exhaustive search for **OPQ**.

- **NAPP** Neighborhood APProximation index ([9], PVLDB’15). 
[Originial source code](https://github.com/searchivarius/nmslib)
    * We disable the SIMD, multi-threading, and `-Ofast` compiler option.

#### 2.2. Tree-Based Space Partitioning Methods

- **FLANN** ([10], TPAMI’14). 
[Originial source code](http://www.cs.ubc.ca/research/flann)
    * We disable the multi-threading techniques in **FLANN**.

- **Annoy** ([11]). 
[Originial source code](https://github.com/spotify/annoy)
    * Source codes are implemented by C++ (core algorithms) and Python (for binding). We re-implemente the Python binding part with C++. 
    * We disable `-ffast-math` compiler option in **Annoy**.

- **VP-Tree** Vantage-Point tree ([12], NIPS’13). 
[Originial source code](https://github.com/searchivarius/nmslib)
    * We disable the SIMD, multi-threading, and `-Ofast` compiler option.

### 3. Neighborhood-Based Methods

- **SW** Small World Graph ([13], IS'14). 
[Originial source code](https://github.com/searchivarius/nmslib)
    * We disable the SIMD, multi-threading, and `-Ofast` compiler option.

- **RCT** Rank Cover Tree ([14], TPAMI'15).
  Originial source code is obtained from authors by email, who kindly allow us to release them through this benchmark. 

- **KGraph** ([15] [16], WWW'11). 
  [Originial source code](https://github.com/aaalgo/kgraph)
  * We disable SIMD and multi-threading techniques in **KGraph**. Note the besides the compiler flag, we also comment out the SIMD related code in `metric.cpp`
  * We reduce the index size of KGraph by discarding the distances of the edges, which is not used in the search process anyway. 

- **DPG** Diversified Proximity Graph ([1]). 
[Originial source code](https://github.com/DBWangGroupUNSW/nns_benchmark/algorithms/DPG)
    We engineered a new algorithm, DPG, that constructs an alternative neighborhood graph index, yet uses the same search algorithm as the KGraph. DPG is more robust than KGraph across different datasets. For more details about the algorithm and its analysis, please refer to our [NNS Experimental Evaluation Paper](http://www.cse.unsw.edu.au/~yingz/NNS.pdf)[1]. 


## Datasets Used 

Currently, we use

- **18 real datasets** that are used by existing work and cover a wide range of application and data types, including image, audio, video, text, and deep-learning data. 
- **2 sythetic datasets**: Rand (for Random) and Gauss (for mixed Gaussian). 

Table 1 summarizes the characteristics of the datasets including the number of data points (*n*), dimensionality (*d*), Relative Contrast (*RC* [18]), local intrinsic dimensionality (*LID* [19]), and data type, where RC and LID are used to describe the hardness of the datasets.

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

We mark the first four datasets in Table 1 with asterisks to indicate that they are **hard datasets** compared with
others.

Below, we give more descriptions of these datasets.

- [**Sift**](http://corpus-texmex.irisa.fr) consists of 1 million 128-d SIFT vectors.

- [**Nusw**](http://lms.comp.nus.edu.sg/research/NUS-WIDE.htm) includes around $2.7$ million web images,
each as a 500-dimensional bag-of-words vector.

- [**Msong**](http://www.ifs.tuwien.ac.at/mir/msd/download.html) is a collection of audio features and metadata for a million contemporary popular music tracks with $420$ dimensions.

- [**Gist**]() is is an image dataset which contains about 1 million data points with 960 dimensions.

- [**Deep**](https://yadi.sk/d/I_yaFVqchJmoc) contains deep neural codes of natural images obtained from the activations of a convolutional neural network, which contains about 1 million data points with 256 dimensions.

- [**Bann**](http://corpus-texmex.irisa.fr/) is used to evaluate the scalability of the algorithms, where 1M, 2M, 4M, 6M, 8M, and 10M data points are sampled from 128-dimensional SIFT descriptors extracted from natural images.

- **Guass** is generated by randomly choosing $1000$ cluster centers with in space $[0,10]^512$, and each cluster follows the a Gaussian distribution with deviation 1 on each dimension.

- **Random** contains $1$M randomly chosen points in a unit hypersphere with dimensionality 100.

- [**Audio**](http://www.cs.princeton.edu/cass/audio.tar.gz) has about 0.05 million 192-d audio feature vectors extracted by Marsyas library from DARPA TIMIT audio speed dataset.

- [**Cifar**](http://www.cs.toronto.edu/~kriz/cifar.html) is a labeled subset of *TinyImage* dataset, which consists of 60000 32 X color images in 10 classes, with each image represented by a 512-d GIST feature vector.

- [**Enron**](http://www.cs.cmu.edu/~enron) origins from a collection of emails. Yifang et. al. extract bi-grams and form feature vectors of 1369 dimensions. 

- [**Glove**](http://nlp.stanford.edu/projects/glove) contains 1.2 million 100-d word feature vectors extracted from Tweets.

- [**Imag**](http://cloudcv.org/objdetect) is introduced and employed by [The ImageNet Large Scale Visual Recognition Challenge (ILSVRC)](http://www.image-net.org/challenges/LSVRC/), which contains about 2.4 million data points with 150-d dense SIFT features.

- [**Mnist**](http://yann.lecun.com/exdb/mnist) consists of 70k images of hand-written digits, each as a 784-d vector concatenating all pixels. we randomly sample 1k as the queries and use the remaining as the data base.
 
- [**Sun**](http://groups.csail.mit.edu/vision/SUN) contains about 0.08 million 512-d GIST features of images.

- [**Notre**](http://phototour.cs.washington.edu/datasets) contains about 0.3 million 128-d features of a set of Flickr images and a reconstruction.

- [**UKbench**](http://vis.uky.edu/~stewe/ukbench) contains about 1 million 128-d features of images.

- [**Trevi**](http://phototour.cs.washington.edu/patches/default.htm) consists of 0.4 million X 1024 bitmap(.bmp) images, each containing a `16 X 16` array of image patch. Each patch is sampled as `64 X 64` grayscale, with a canonical scale and orientation. Therefore, Trevi patch dataset consists of around 100,000 4096-d vectors.

- [**UQ_V**](http://staff.itee.uq.edu.au/shenht/UQ_VIDEO/) is a video dataset. A
  local feature based on some keyframes are extracted which include 256
  dimensions.
  
- [**Yout**](http://www.cs.tau.ac.il/~wolf/ytfaces/index.html) contains 3,425 videos of 1,595 different people. All the videos were downloaded from YouTube. An average of 2.15 videos are available for each subject. The shortest clip duration is 48 frames, the longest clip is 6,070 frames, and the average length of a video clip is 181.3 frames.

## PERFORMANCE EVALUATION and ANALYSES 

Please refer to our [NNS Experimental Evaluation paper](http://www.cse.unsw.edu.au/~yingz/NNS.pdf)[1]. 

## License

Our own code is released under the [Apache License Version 2.0](http://www.apache.org/licenses/). Copyright is owned by DBWang Group (University of New South Wales, Australia), Wen Li, and Ying Zhang. 

Below are the license information for the included implementations: 

1. KGraph: BSD license. Users are still encouraged to download the binary distributions from [its home site](https://github.com/aaalgo/kgraph) so building issues can be avoided.

2. NMSLib and Annoy: [Apache License Version 2.0](http://www.apache.org/licenses/).

3. AGH: License information below

```
Terms of Use
--
Copyright (c) 2009-2011 by
--
DVMM Laboratory
Department of Electrical Engineering
Columbia University
Rm 1312 S.W. Mudd, 500 West 120th Street
New York, NY 10027
USA
--
If it is your intention to use this code for non-commercial purposes, such as in academic research, this code is free.
--
If you use this code in your research, please acknowledge the authors, and cite our related publication:
--

Wei Liu, Jun Wang, Sanjiv Kumar, and Shih-Fu Chang, "Hashing with Graphs," International Conference on Machine Learning (ICML), Bellevue, Washington, USA, 2011
```

4. SRS: GPL License.

5. FLANN: BSD License. 

6. RCT: The Authors grant us the permission to release source code by email.

7. Algorithms whose license information are not mentioned: NSH,  OPQ, QALSH, SGH, and SH. 

 
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








