
data_dir="../../data"
index_dir="../../index/kgraph"
result_dir="results"

k=20

L=40

mkdir results 

for P in 1 5 10 20 40 50 100
do
for data in audio  
do

kgraph_search --data ${data_dir}/${data}_base.lshkit --query  ${data_dir}/${data}_query.lshkit --eval ${data_dir}/${data}_groundtruth.lshkit -K ${k} --index ${index_dir}/${data}.kgraph_${L} -P ${P} 

done
done

