data_path=../../../data
index_path=../index
mkdir ${index_path}

for data in "audio"
do
index_path="${index_path}/${data}"
mkdir ${index_path}

n=53387
nq=200
d=192
m=8

cd ../src

echo "== Calculating parameter =="
./cal_param -n $n -m $m

echo "== Indexing... =="
./srs -I -d $d -i ${index_path}/ -m $m -n $n -s "${data_path}/${data}_base.txt" -p ${nq} -y f

done

 
