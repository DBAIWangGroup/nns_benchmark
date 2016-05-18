# DATA used in the NNS Benchmark 

Below are Dropbox share links for some of datasets evaluated in the benchmark.
Data downloaded should be located at *nns_benchmark/data* directory.

- Audio ([data](https://www.dropbox.com/s/teqte5esxz0j0bx/audio_base.fvecs?dl=0), [query](https://www.dropbox.com/s/4r4tu5y5623i41v/audio_query.fvecs?dl=0), [groud truth](https://www.dropbox.com/s/gezm7hq2yryttcq/audio_groundtruth.ivecs?dl=0))
- Sift ([data](https://www.dropbox.com/s/vb6w935xy1u7f5l/sift_base.fvecs?dl=0), [query](https://www.dropbox.com/s/kd3cgmivfi9rr1f/sift_query.fvecs?dl=0), [groud truth](https://www.dropbox.com/s/m3ud3s8z2dwn42p/sift_groundtruth.ivecs?dl=0))

### Data format 
Please refer to [corpus-texmex](http://corpus-texmex.irisa.fr/) for the details of the formats.
Particularly, we use *.fvecs* format for data points and query points, and *.ivecs* format for ground truth.


### REMARK
We removed the duplications of the data points after downloaded from corresponding websites.
For each dataset, we reserved 200 data points as the query points. The ground truth results are provided for each dataset.
