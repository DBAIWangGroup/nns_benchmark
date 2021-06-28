# Datasets Used in the NNS Benchmark

For each dataset (e.g., audio), we have files for data points (e.g., `audio_base.lshkit`), query points (e.g., `audio_query.lshkit`) and ground truth files (e.g., `audio_groundtruth.lshkit`). Five different file formats are used by existing algorithms evaluated, including [*fvecs*,*ivecs*](http://corpus-texmex.irisa.fr), [*lshkit*](http://www.kgraph.org/index.php?n=Main.LshkitFormat), [*hdf5*](https://github.com/mariusmuja/flann) and [*txt*](https://github.com/DBWangGroupUNSW/SRS) (plain text).

Please **download** the following compressed dataset as the running sample data used in our examples. Datasets downloaded should be unzipped and located in the `./nns_benchmark/data` directory.

- [Audio](https://www.dropbox.com/s/1hhaz08i3qk4ett/audio.zip?dl=0). Audio contains about 0.05 million 192-d audio feature vectors extracted by Marsyas library from DARPA TIMIT audio speed dataset.
- [Cifar](https://www.dropbox.com/s/pcliqe2cl4qc5ff/cifar.hdf5?dl=0). Cifar is a labeled subset of TinyImage dataset, which consists of 60000 32 × color images in 10 classes, with each image represented by a 512-d GIST feature vector.
- [Deep](https://www.dropbox.com/s/y9c0vf9jac7yax8/deep.hdf5?dl=0). Deep dataset contains deep neural codes of natural images obtained from the activations of a convolutional neural network, which contains about 1 million data points with 256 dimensions.
- [Enron](https://www.dropbox.com/s/z56uf5qdmpp6iqo/enron.hdf5?dl=0). Enron origins from a collection of emails. yifang et. al. extract bi-grams and form feature vectors of 1369 dimensions.
- [Glove](https://www.dropbox.com/s/xg0jvdnp8oszhuu/glove.hdf5?dl=0). Glove contains 1.2 million 100-d word feature vectors extracted from Tweets.
- [ImageNet](https://www.dropbox.com/s/w8qj7w8lloify5y/imageNet.hdf5?dl=0). ImageNet is introduced and employed by “The ImageNet Large Scale Visual Recognition Challenge(ILSVRC)”, which contains about 2.4 million data points with 150 dimensions dense SIFT features.
- [Msong](https://www.dropbox.com/s/mh11y5q7dugehwi/millionSong.hdf5?dl=0). Msong is a collection of audio features and metadata for a million contemporary popular music tracks with 420 dimensions.
- [Mnist](https://www.dropbox.com/s/wg25xib5iiszu4k/MNIST.hdf5?dl=0). Mnist consists of 70k images of hand-written digits, each as a 784-d vector concatenating all pixels. we randomly sample 1k as the queries and use the remaining as the data base.
- [Notre](https://www.dropbox.com/s/v876ggkmodvr4xo/notre.hdf5?dl=0). Notre contains about 0.3 million 128-d features of a set of Flickr images and a reconstruction.
- [Nusw](https://www.dropbox.com/s/7yxx17vig60yxsl/nuswide.hdf5?dl=0). Nusw includes around 2.7 million web images, each as a 500-dimensional bag-of-words vector.
- [Random](https://pan.baidu.com/s/1o8PPSfC). Random contains 1M randomly chosen points in a unit hypersphere with dimensionality 100.
- [Sift](https://www.dropbox.com/s/vbdb0imp83t6pb1/sift.hdf5?dl=0). Sift consists of 1 million 128-d SIFT vectors.
- [Sun397](https://www.dropbox.com/s/h8lvtvfbejghi99/sun.hdf5?dl=0). Sun397 contains about 0.08 million 512-d GIST features of images.
- [Trevi](https://www.dropbox.com/s/9ezi2gkuhnkem6d/trevi.hdf5?dl=0). Trevi consists of 0.4 million × 1024 bitmap(.bmp) images, each containing a 16 × 16 array of image patches. Each patch is sampled as 64 × 64 grayscale, with a canonical scale and orientation. Therefore, Trevi patch dataset consists of around 100,000 4096-d vectors.
- [UKbench](https://www.dropbox.com/s/frra3z1bi7otjo5/ukbench.hdf5?dl=0). UKbench contains about 1 million 128-d features of images.

More datasets evaluated in our [NNS Experimental Evaluation paper](https://arxiv.org/abs/1610.02455) will be uploaded soon. 

## Deduplication and Query Set Generation

In all of our datasets, we *have already removed the duplicate data points* from the original ones downloaded from the corresponding source websites. For each dataset, by default, we reserved 200 random data points as the query points. The ground truth results are also provided for each dataset.
