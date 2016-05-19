index_path=$1
query_path=$2 # ./fvecs
gnd_path=$3 # ./ivecs
k=$4
output_path=$5

cd ../src

for stop in 100 300 400 500 800 1000
do

./searcher -i ${index_path} -q ${query_path} -g ${gnd_path} -o ${output_path} -k ${k} -c ${stop}  
done


