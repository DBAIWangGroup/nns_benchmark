data_dir="../../../data"
index_dir="../index"
bin_dir="../src"

mkdir ${index_dir} 


# Please sure the data ( data points, query points and ground truth files are located at ../../../data directory )

for data in audio # sift 
do

# L is for lengh of the NN-list (i.e.,#neighbors materialized) of the  K-NN graph
# the K value of the input is the desirable k value in the k-NN search 

L=40

${bin_dir}/kgraph_index --data ${data_dir}/${data}_base.lshkit --output ${index_dir}/${data}.kgraph_${L} -S 20 -L ${L} -K 20

done


