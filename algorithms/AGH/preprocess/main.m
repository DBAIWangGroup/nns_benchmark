% data_path: the full path of the dataset
% m: the number of anchor
% s: the number of the nearest anchors need to be considered
% bit: the length of the hashcode

data_name='audio';
data_path=['../../../data/' data_name '_base.fvecs'];
    
index_path = ['../index/' data_name];
mkdir(index_path);

anchor_path=[index_path '/' data_name '_anchor'];
L1_params_path=[index_path '/' data_name '_params.L1']; % the transformation parameters for 1-AGH
L1_data_path=[index_path '/' data_name '_B1.L1'];% the binary format of the dataset

L2_params_path=[index_path '/' data_name '_params.L2'];
L2_data_path=[index_path '/' data_name '_B1.L2'];

m=300;
s=5;
bit=8;

get_anchor(data_path,anchor_path,m);
main_OneLayer(data_path,anchor_path,L1_params_path,L1_data_path,s,bit);
main_TwoLayer(data_path,anchor_path,L2_params_path,L2_data_path,s,bit);
