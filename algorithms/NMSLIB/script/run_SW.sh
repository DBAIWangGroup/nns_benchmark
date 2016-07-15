data_path=../../../data
query_path=../../../data
result_path=../results
mkdir ${result_path}
mkdir "../indices"

for data in "audio"
do
n=53387
m=200
K=20
NN=10
I=5

dataset=${data_path}/${data}/${data}_base.txt
query=${data_path}/${data}/${data}_query.txt
result_path=${result_path}/${data}_SW_${NN}NN_${I}I.txt

Str="--queryTimeParams initSearchAttempts=1,efSearch=1 "
for i in 2 3 4 5 6 8 10 15 20 30 50 70 80 100 150 200 250 300 500 700 1000 1500 1700 2000 2500
do
Str="${Str} -t initSearchAttempts=1,efSearch=$i  "
done

../code/release/experiment \
--distType float --spaceType l2 \
--knn 20 \
--dataFile ${dataset} --maxNumData $n --queryFile ${query} --maxNumQuery 200 -o ${result_path} -a \
--method sw-graph \
--createIndex NN=${NN},indexThreadQty=41 \
--saveIndex ../indices/${data}_${NN}NN_${I}I.sw \
${Str}

done
 
