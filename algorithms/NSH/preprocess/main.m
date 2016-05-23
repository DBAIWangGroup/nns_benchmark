data_name='audio';
data_path=['../../../data/' data_name '_base.fvecs'];

index_path=['../index/' data_name];
mkdir(index_path);

params_path=[index_path '/' data_name '_params.NSH'];
dataset_binary_path=[index_path '/' data_name '_B1.NSH'];
bit=8;

run_NSH(data_path, params_path, dataset_binary_path, bit);
