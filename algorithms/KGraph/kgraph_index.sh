data_dir="../../data"
index_dir="../../index/kgraph"

mkdir ${index_dir} 

# L is for lengh of the NN-list (i.e.,#neighbors materialized) of the  K-NN graph  
for L in 20 40
do

for data in audio sift 
do

kgraph_index --data ${data_dir}/${data}_base.lshkit --output ${index_dir}/${data}.kgraph_${L} -S 20 -L ${L} -K 20

done
done


