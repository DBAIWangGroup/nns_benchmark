
data_path="../../../data"
result_path="../results"
mkdir ${result_path}

cd ../src

k=20

for data in "audio"
do

dataset_path="${data_path}/${data}_base.fvecs"
query_path="${data_path}/${data}_query.fvecs"
gnd_path="${data_path}/${data}_groundtruth.ivecs"
 
for layer in 1 2         # the layer of AGH
do

params_path=../index/${data}/${data}_params.L${layer}   # file stored the transformation parameters
dataset_binary_path=../index/${data}/${data}_B1.L${layer}  # the file stored the binary dataset
nbit=8          #  the length of the hash code
m=300            # the number of the anchors
t=5             # the number of the nearest anchors used to build the app. graph
radis=-2          # searching limited by the number of the maximum visited points

for c in 100 500 1000
do

./AGH -s ${dataset_path} -q ${query_path} -g ${gnd_path} -k $k -a $m -t $t -l ${layer} -c $c -r ${radis} -b "${dataset_binary_path}" -p "${params_path}" >> "${result_path}/${data}_${layer}AGH.txt"

done
done
done

