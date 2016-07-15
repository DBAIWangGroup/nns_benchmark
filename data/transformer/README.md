# For each dataset, five different file formats are used to evaluate the algorithms: 
	- .fvecs/ivecs ( used in AGH, Annoy, NSH, OPQ, RCT and SGH);
	- .lshkit ( used in SH, KGraph, DPG);
	- .hdf5 ( used in flann);
	- .txt ( used in SRS, QALSH, HNSW, SW, vptree, NAPP).

Due to .hdf5 is a file format for storing and managing the data collections of all sizes and complexity, we only provide the hdf5 file for all the datasets and give a data transformation tool to save the data, query and ground truth files to other formats. In addition, we also offer a tool to compute the ground truth and save to the hdf5 file. In this application, hdf5 file consists of three datasets with the names "dataset", "query" and "groundtruth". Each coordinate of the data and query is a floating point number and the ground truth file only contain the IDs of top k points.


## Compile 

go to Converter/src and run

```
make
```

to get the executable file.

## Save to other formats

Use load_from_hdf to extract the data and save to other types.

% ./load_from_hdf -f {filename} -n {name} -o {output_name} -t {file format} -y {data type}

	-f: the path of the hdf5 file;
	-n: the name of the dataset( one hdf5 file can contain several dataset);
	-o: the path of the output file;
	-t: the format of the output file, which could be chosen from "fvecs/ivecs/txt/lshkit".
	-y: the type of the stored data. -y i indicates that each coordinate is an integer. The other option is -y f, indicating that the saving objects are the data or query points.


% ./compute_groundtruth -f {filename} -k {k}

	-f: the path of the hdf5 file, which must contain data and query dataset;
	-k: the number of the nearest neighbors. The results are stored in the same file and labeled as the name "groundtruth".

 

