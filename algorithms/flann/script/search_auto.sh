data_path=../../../data
index_path=../index
result_path=../results
mkdir ${result_path}
k=20

for data in "audio"
do

for c in 100 500 1000
do

../code/build/bin/search_auto -f ${data_path}/${data}.hdf5 -i ${index_path}/${data}.auto -r ${result_path}/${data}_auto.txt -k $k -c ${c}

done
done
