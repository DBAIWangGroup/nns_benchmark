function coarse_vocabularies(yael_path,file_path,Rinit_path, coarse_name,K)

%addpath ('~/Documents/yael/yael_v401/matlab');
all_data = fvecs_read(file_path);
all_data = all_data';

dim=size(all_data,2);
niter=30;
M=2;

tic;
%%%OPQ_NP
R_init = eye(dim);
all_data = single(all_data);
vocabSize = 2^K;
% % add implementation of K-means

center_table_init = cell (M,1);

[centers_table_opq_np R_opq_np] = train_opq_np(all_data,M,center_table_init,R_init,niter/2,1,vocabSize);
vocab1 = centers_table_opq_np{1}';
vocab2 = centers_table_opq_np{2}';

fvecs_write(Rinit_path,R_opq_np);
file = fopen([coarse_name '.dat'], 'w');
dim = size(vocab1, 1);
sz = size(vocab1, 2);
fwrite(file, dim, 'int32');
fwrite(file, sz, 'int32');
fwrite(file, vocab1, 'float');
fwrite(file, vocab2, 'float');
fclose(file);
save([coarse_name '.mat'], 'vocab1', 'vocab2');
time=toc;
end
