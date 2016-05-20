data_path=../../../data
index_path=../index
mkdir ${index_path}

sampleRate=4
parent=5
scale=5

for data in "audio"
do

../src/indexer -s ${data_path}/${data}_base.fvecs -i ${index_path}/${data} -p ${parent} -f ${scale} -r ${sampleRate} 

done




