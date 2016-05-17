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

Currently, we evaluate 15 representative NNS algorithms on 20 datasets where details are reported in [1] .

Below are brief introuductions of the algorithms evaluated in the benchmark.

### ALGORITHMS EVALUTED 


### Experiment paper 

We recommend our recent research paper[1], which conduct comprehensive experimental study of state-of-the-art NNS methods across several different research areas. Our contribution include:

- comparing all the methods without adding any implementation tricks, which makes the comparison more fair; 
- evaluating all the methods using multiple measures; and
- providing a deeper understanding of the state-of-the-art with new insights into the strength and weakness of each method. We also provide some suggestions about how to select the method under different settings.
- providing rule-of-the-thumb recommendations about how to select the method under different settings.

- We group algorithms into several categories, and then perform detailed analysis on both intra- and inter-category evaluations. Our data-based analyses provide confirmation of useful principles to solve the problem, the strength and weakness of some of the best methods, and some initial explanation and understanding of why some datasets are harder than others. The   experience and insights we gained throughout the study enable us to engineer a  new empirical algorithm, DPG, that  outperforms all the existing algorithms in majority of the settings and is the most robust in most aspects.

[1] Nearest Neighbor Search on High Dimensional Data — Experiments, Analyses, and Improvement [Full Version] http://www.cse.unsw.edu.au/~yingz/NNS.pdf

