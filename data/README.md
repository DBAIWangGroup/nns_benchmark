# Datasets Used in the NNS Benchmark

For each dataset (e.g., audio), we have files for data points (e.g., audio_base.lshkit, query points (e.g., audio_query.lshkit) 
and ground truth files (e.g., audio_groundtruth.lshkit). Five different file formats are used by existing algorithms evaluated,
including [*fvecs*,*ivecs*](http://corpus-texmex.irisa.fr), [**lshkit**](http://www.kgraph.org/index.php?n=Main.LshkitFormat), [*hdf5*](https://github.com/mariusmuja/flann) and [*txt*](https://github.com/DBWangGroupUNSW/SRS) (plain text).

Please **download** the following compressed dataset as the running sample data used in our examples. Datasets downloaded should be unzipped and located in the `./nns_benchmark/data` directory.

- [Audio] (https://www.dropbox.com/s/1hhaz08i3qk4ett/audio.zip?dl=0). Audio contains about 0.05 million 192-d audio feature vectors extracted by Marsyas library from DARPA TIMIT audio speed dataset.

More datasets evaluated in our [experimental evaluation paper](http://www.cse.unsw.edu.au/~yingz/NNS.pdf) will be uploaded soon. 

## Deduplication and Query Set Generation

In all of our datasets, we *have already removed the duplicate data points* from the original ones downloaded from the corresponding source websites. For each dataset, we reserved 200 random data points as the query points. The ground truth results are also provided for each dataset.
