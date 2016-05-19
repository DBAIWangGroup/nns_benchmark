
data_dir="../../../data"
index_dir="../index"
bin_dir="../src"
result_dir="../results"


mkdir ${result_dir}


k=20
L=20


for data in audio  
do

for P in 1 2 5 10 20 40 50 100
do

# show the result 
${bin_dir}/DPG_search --data ${data_dir}/${data}_base.lshkit --query  ${data_dir}/${data}_query.lshkit --eval ${data_dir}/${data}_groundtruth.lshkit -K ${k} --index ${index_dir}/${data}.DPG_${L} -P ${P} 

# try again and keep the results into files 

result="${data}_k${k}_P${P}.txt" 

${bin_dir}/DPG_search --data ${data_dir}/${data}_base.lshkit --query  ${data_dir}/${data}_query.lshkit --eval ${data_dir}/${data}_groundtruth.lshkit -K ${k} --index ${index_dir}/${data}.DPG_${L} -P ${P} > ${result_dir}/${result}

done
done



