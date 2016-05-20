data_path=../../../data
query_path=../../../data
gnd_path=../../../data
index_path=../index
result_path=../results
mkdir ${result_path}

k=20

for data in "audio"
do
index_path="${index_path}/${data}"

n=53387
nq=200
d=192
m=8

cd ../src

for c in 100 500 1000
do

./srs -Q -c 4 -g ${gnd_path}/${data}_groundtruth.txt -i ${index_path}/ -k $k -q ${query_path}/${data}_query.txt -t ${c} -r 1.315558 -p ${nq} -y f -d $d -m ${m} -n $n -o ${result_path}/${data}.txt

done
done

 
