data_name='audio';
data_path=['../../../data/' data_name '_base.fvecs'];

index_path=['../index/' data_name];
mkdir(index_path);

params_path=[index_path '/' data_name '_params.SGH'];
binary_path=[index_path '/' data_name '_B1.SGH'];

bit=8;
run_SGH(data_path, params_path, binary_path, bit);
