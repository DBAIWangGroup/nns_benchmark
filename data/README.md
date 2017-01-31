# Datasets Used in the NNS Benchmark

For each dataset (e.g., audio), we have files for data points (e.g., `audio_base.lshkit`), query points (e.g., `audio_query.lshkit`) and ground truth files (e.g., `audio_groundtruth.lshkit`). Five different file formats are used by existing algorithms evaluated, including [*fvecs*,*ivecs*](http://corpus-texmex.irisa.fr), [*lshkit*](http://www.kgraph.org/index.php?n=Main.LshkitFormat), [*hdf5*](https://github.com/mariusmuja/flann) and [*txt*](https://github.com/DBWangGroupUNSW/SRS) (plain text).

Please **download** the following compressed dataset as the running sample data used in our examples. Datasets downloaded should be unzipped and located in the `./nns_benchmark/data` directory.

- [Audio](https://www.dropbox.com/s/1hhaz08i3qk4ett/audio.zip?dl=0). Audio contains about 0.05 million 192-d audio feature vectors extracted by Marsyas library from DARPA TIMIT audio speed dataset.
- [Cifar](https://nj01ct01.baidupcs.com/file/3c5163bc954d7948f3eb4075c49b0399?bkt=p3-000083efd09815b00d546431b087447b3f0b&fid=4061005705-250528-1011304430138055&time=1485838452&sign=FDTAXGERLBH-DCb740ccc5511e5e8fedcff06b081203-ivLe22mYXJbZRoMnue4SbAevwP0%3D&to=njhb&fm=Yan,B,U,ny&sta_dx=206505064&sta_cs=2&sta_ft=hdf5&sta_ct=3&sta_mt=3&fm2=Yangquan,B,U,ny&newver=1&newfm=1&secfm=1&flow_ver=3&pkey=000083efd09815b00d546431b087447b3f0b&sl=69926991&expires=8h&rt=sh&r=358801100&mlogid=708254097456682051&vuk=4061005705&vbdid=1214621705&fin=cifar.hdf5&fn=cifar.hdf5&slt=pm&uta=0&rtype=1&iv=0&isw=0&dp-logid=708254097456682051&dp-callid=0.1.1&hps=1&csl=500&csign=VCqN6lpkt367mjKc22K7F%2BejUwY%3D). Cifar is a labeled subset of TinyImage dataset, which consists of 60000 32 × color images in 10 classes, with each image represented by a 512-d GIST feature vector.
- [Deep](https://pan.baidu.com/s/1eR2juO6). Deep dataset contains deep neural codes of natural images obtained from the activations of a convolutional neural network, which contains about 1 million data points with 256 dimensions.

More datasets evaluated in our [NNS Experimental Evaluation paper](https://arxiv.org/abs/1610.02455) will be uploaded soon. 

## Deduplication and Query Set Generation

In all of our datasets, we *have already removed the duplicate data points* from the original ones downloaded from the corresponding source websites. For each dataset, by default, we reserved 200 random data points as the query points. The ground truth results are also provided for each dataset.
