data_path=../../../data
index_path=../index

mkdir ${index_path}
iteration=5
branches=64

for data in "audio"
do

../code/build/bin/build_hkmeans -f ${data_path}/${data}.hdf5 -i ${index_path}/${data}.hkmeans -t ${iteration} -b ${branches}

done

