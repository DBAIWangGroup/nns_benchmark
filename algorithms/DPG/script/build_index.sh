data_dir="../../../data"
index_dir="../index"
bin_dir="../src"

mkdir ${index_dir} 

# Please sure the data ( data points, query points and ground truth files are located at ../../../data directory )

# L1 and L2  is the length of the NN-list used by K-NN graph and DPG 

L1=40
L2=20

for data in audio 
do


# first construct K-NN graph

${bin_dir}/DPG_index --data ${data_dir}/${data}_base.lshkit --output ${index_dir}/${data}.kgraph_${L1} -S 20 -L ${L1} -K 20

# do the diversification 

${bin_dir}/DPG_diverse --data ${data_dir}/${data}_base.lshkit --index ${index_dir}/${data}.kgraph_${L1}  --output ${index_dir}/${data}.DPG_${L2}  -L ${L2}


done


