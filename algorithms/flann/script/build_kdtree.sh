data_path=../../../data/
index_path=../index

mkdir ${index_path}
trees=4

for data in "audio"
do

../code/build/bin/build_kdtree -f ${data_path}/${data}.hdf5 -i ${index_path}/${data}.kdtree -t $trees

done
