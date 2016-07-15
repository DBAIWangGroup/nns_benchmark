data_path=../../../data
query_path=../../../data
result_path=../results
mkdir ${result_path}

for data in "audio"
do
n=53387
m=200
K=20
bucket_size=100

dataset=${data_path}/${data}/${data}_base.txt
query=${data_path}/${data}/${data}_query.txt
result_path=${result_path}/${data}_vptree_${bucket_size}B.txt

for i in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 0.95
do
../code/release/experiment \
--distType float --spaceType l2 \
--knn $K \
--dataFile ${dataset} --maxNumData $n --queryFile ${query} --maxNumQuery 200 -o ${result_path} -a \
--method vptree \
--createIndex bucketSize=${bucket_size},desiredRecall=$i,tuneK=$K,chunkBucket=1
done
done
