data_path="../../../data"
result_path="../results"
mkdir ${result_path}

for data in "audio"
do

dataset_path=${data_path}/${data}_base.fvecs
query_path=${data_path}/${data}_query.fvecs
gnd_path=${data_path}/${data}_groundtruth.ivecs
params_path=../index/${data}/${data}_params.SGH
dataset_binary_path=../index/${data}/${data}_B1.SGH
k=20
radis=-2

for c in 100 500 1000
do

../src/SGH -s ${dataset_path} -q ${query_path} -g ${gnd_path} -c $c -t 300 -k $k -r ${radis} -b "${dataset_binary_path}" -p "${params_path}" >> "${result_path}/${data}.txt"

done

done
