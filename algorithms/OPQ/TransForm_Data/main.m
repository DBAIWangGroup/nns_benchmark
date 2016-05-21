n=1000;
data_name='audio';
K=8;
M=8;

data_path='../../../../data';
index_path=['../index/' data_name];
mkdir(index_path);

yael_path = '/home/yzhang4/HD/home/liwen/Documents/yael/yael_v401';
vlfeat_path='/home/yzhang4/HD/home/liwen/Downloads/software/vlfeat-0.9.20';

generate_learn([data_path '/' data_name '_base.fvecs'],[index_path '/' data_name '_learn.fvecs'],n);

coarse_vocabularies(yael_path, [index_path '/' data_name '_learn.fvecs'],[index_path '/' data_name '_rinit.fvecs'],[index_path '/' data_name '_coarse'],K);

fine_vocabularies(yael_path,vlfeat_path, [index_path '/' data_name '_learn.fvecs'], [index_path '/' data_name '_rinit.fvecs'], [index_path '/' data_name '_coarse'], [index_path '/' data_name '_fine'], K, M);


transform_base_query([data_path '/' data_name '_base.fvecs'], [data_path '/' data_name '_query.fvecs'], [index_path '/' data_name '_rinit.fvecs'], [index_path '/' data_name '_base_NP.fvecs'], [index_path '/' data_name '_query_NP.fvecs']);



