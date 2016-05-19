data_path=$1 # ./fvecs
trees=$2     # the number of tree
index_path=$3    
indextime_path=$4

cd ../src

./indexer -i ${index_path} -d ${data_path} -o ${indextime_path} -t ${trees}
