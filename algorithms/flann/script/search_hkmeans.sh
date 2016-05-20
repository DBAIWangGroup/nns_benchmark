data_path=../../../data/
index_path=../index
result_path=../results

mkdir ${result_path}
k=20

for data in "audio"
do

for c in 100 500 1000
do

../code/build/bin/search_hkmeans -f ${data_path}/${data}.hdf5 -r ${result_path}/${data}_hkmeans.txt -i ${index_path}/${data}.hkmeans -k $k -c $c

done
done
