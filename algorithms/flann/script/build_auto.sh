data_path=../../../data
index_path=../index
mkdir ${index_path}
wm=0
wb=0.01
sample=0.1
target=0.9

for data in "audio"
do

../code/build/bin/build_auto -f ${data_path}/${data}.hdf5 -i ${index_path}/${data}.auto -m ${wm} -b ${wb} -s ${sample} -t ${target}

done
