result_path=../results
mkdir ${result_path}

for data  in "audio"
do

data_path=../index/${data}/${data}_base_NP.fvecs
query_path=../index/${data}/${data}_query_NP.fvecs
gnd_path=../../../data/${data}_groundtruth.ivecs
coarse_path=../index/${data}/${data}_coarse.dat
fine_path=../index/${data}/${data}_fine.dat
index_path=../index/${data}/
n=53387
dim=192
nq=200
k=20
use_residual=1
rerank=0
use_original=1
K=8
let centroids=2**$K

for c in 100 500 1000
do

../src/build_master/searcher_tester \
--index_files_prefix=${index_path} \
--queries_file=${query_path} \
--queries_count=${nq} \
--neighbours_count=$c \
--groundtruth_file=${gnd_path} \
--data_file ${data_path} \
--data_count ${n} \
--coarse_vocabs_file=${coarse_path} \
--fine_vocabs_file=${fine_path} \
--query_point_type="FVEC" \
--use_residuals ${use_residual} \
--do_rerank ${rerank} \
--report_file=${result_path}/${data}.txt \
--dim ${dim} \
--use_originaldata ${use_original} \
--subspaces_centroids_count=${centroids} \
--k_neighbors $k \

done
done



