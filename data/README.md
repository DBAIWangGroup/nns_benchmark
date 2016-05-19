# DATA used in the NNS Benchmark 

For each dataset (e.g., audio), we have files for data points (e.g., audio_base.lshkit, query points (e.g., audio_query.lshkit) 
and ground truth files (e.g., audio_groundtruth.lshkit). Three different file formats are used by existing algorithms evaluated ,
including [**fvecs**](http://corpus-texmex.irisa.fr), [**lshkit**]() and [**txt**] (plain text).


Please **download** the following compressed audio data with three format as the sample data used in our examples.
Data downloaded should be unzipped and located at **nns_benchmark/data** directory.

- [Audio] (https://www.dropbox.com/s/1hhaz08i3qk4ett/audio.zip?dl=0). Audio has about 0.05 million 192-d audio feature vectors extracted by Marsyas library from DARPA TIMIT audio speed dataset.

More dataset will be uploaded soon. 

### REMARK
We removed the duplications of the data points after downloaded from corresponding websites.
For each dataset, we reserved 200 data points as the query points. The ground truth results are provided for each dataset.
