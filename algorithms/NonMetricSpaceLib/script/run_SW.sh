data_path=../../../data
query_path=../../../data
gnd_path=../gnd
mkdir ${gnd_path}
result_path=../results
mkdir ${result_path}

for data in "audio"
do

n=53387
nq=200
k=20
NN=10
initIndex=5

T=1
methodStr="--method"

for initSearch in 1 2 3 4 5 7 8 9 10 15 20 50 70 100
do

methodStr="$methodStr small_world_rand:NN=${NN},initSearchAttempts=${initSearch},initIndexAttempts=${initIndex},indexThreadQty=${T}"

done
echo "run"
../code/release/experiment \
--distType float --spaceType l2 \
--knn $k \
--dataFile ${data_path}/${data}_base.txt --maxNumData $n --queryFile ${query_path}/${data}_query.txt --maxNumQuery $nq --cachePrefixGS ${gnd_path}/${data} \
${methodStr} >> ${result_path}/${data}_SW.txt

done

