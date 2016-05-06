function coarse_vocabularies(file_path,data_name,K,s)

addpath ('~/Documents/yael/yael_v401/matlab')
all_data = fvecs_read(['~/Downloads/Dataset/' file_path '/learn/' data_name '_learn_' num2str(s) '.fvecs']);
all_data = all_data';

dim=size(all_data,2);

type = 'NP';

niter=30;
M=2;

tic;
%%%OPQ_NP
R_init = eye(dim); %eigenvalue_allocation(all_data', M);

all_data = single(all_data);
vocabSize = 2^K;
% % add implementation of K-means

center_table_init = cell (M,1);

[centers_table_opq_np R_opq_np] = train_opq_np(all_data,M,center_table_init,R_init,niter/2,1,vocabSize);
vocab1 = centers_table_opq_np{1}';
vocab2 = centers_table_opq_np{2}';



fvecs_write(['~/Documents/MultiIndex-master-OPQ/R_init/' data_name '_' num2str(vocabSize) type '.fvecs'],R_opq_np);
file = fopen(['~/Documents/MultiIndex-master-OPQ/coarse_quantization/' data_name '_double_' num2str(vocabSize) '_' type '.dat'], 'w');
dim = size(vocab1, 1);
sz = size(vocab1, 2);
fwrite(file, dim, 'int32');
fwrite(file, sz, 'int32');
fwrite(file, vocab1, 'float');
fwrite(file, vocab2, 'float');
fclose(file);
save(['~/Documents/MultiIndex-master-OPQ/coarse_quantization/' data_name '_double_' num2str(vocabSize) '_' type '.mat'], 'vocab1', 'vocab2');
time=toc;

fid=fopen('preprocess.txt','a');
fprintf(fid,'%.6f #coarse_time #K_%d %s \n',time, K, data_name);
fclose(fid);

end
