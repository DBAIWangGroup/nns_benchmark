%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% SGH demo
% Author: Qing-Yuan Jiang
% Mail: jiangqy@lamda.nju.edu.cn  or  qyjiang24@gmail.com
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Initialization
% We assume that file TINY.mat contains the variable XTrain and XTest. Where
% XTrain and XTest are sampled from original dataset and are scaled. 
clear;

path_name='MNIST';
data_name='MNIST';

XTrain = fvecs_read(['/home/liwen/Downloads/Dataset/' path_name '/data/' data_name '_base.fvecs']);
XTest = fvecs_read(['/home/liwen/Downloads/Dataset/' path_name '/query/' data_name '_query.fvecs']);
XTrain = double(XTrain');
XTest = double(XTest');
Xgnd=ivecs_read(['/home/liwen/Downloads/Dataset/' path_name '/gnd/' data_name '_groundtruth.ivecs']);

result_file=['/home/liwen/Downloads/Dataset/' path_name '/exp_fig/result/SGH/SGH_precision.txt'];
fid_precision=''; %=fopen(result_file,'w');

result_file=['/home/liwen/Downloads/Dataset/' path_name '/exp_fig/result/SGH/SGH.txt'];
fid_recall=''; %=fopen(result_file,'w');

result_file=['/home/liwen/Downloads/Dataset/' path_name '/exp_fig/result/SGH/SGH_reranking.txt'];
fid_reranking=''; %=fopen(result_file,'w');

bit = [16,32,64,96,128,256];%
nb = size(bit,2);
for i = 1:nb
    SGH(XTrain,XTest,bit(i),Xgnd,fid_precision,fid_recall,fid_reranking);
end
fclose(fid_precision);
