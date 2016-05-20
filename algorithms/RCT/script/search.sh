data_path=../../../data
query_path=../../../data
gnd_path=../../../data
indices_path=../index
result_path=../results
mkdir ${result_path}

scale=5
k=20

for data in "audio"
do

for c in 0.5 0.7 0.9 1 1.2 1.3 1.4 1.5 2.0 2.3 2.5 3 4 5 6
do

if [ `expr $scale \> $c` -eq 1 ];then

../src/searcher -s ${data_path}/${data}_base.fvecs -q ${query_path}/${data}_query.fvecs -g ${gnd_path}/${data}_groundtruth.ivecs -i ${indices_path}/${data} -o ${result_path}/${data}.txt -k ${k} -c $c

else
echo $c
fi

done
done




