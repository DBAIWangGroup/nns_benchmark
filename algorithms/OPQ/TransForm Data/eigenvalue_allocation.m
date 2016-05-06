function R = eigenvalue_allocation(X, M)

% Learn a projecting matrix R before product quantization

% This is the EigenValue Allocation in our CVPR 2013 paper:
% "Optimized Product Quantization for Approximate Nearest Neighbor Search"
% by Tiezheng Ge, Kaiming He, Qifa Ke, and Jian Sun.

% Input:
%   X = training data n-by-d
%   m = num of subspaces

% Output:
%   R = a d-by-d orthogonal matrix

n = size(X, 1);    %输入数据条数
dim = size(X, 2);  %数据维度

%%% remove mean
sample_mean = mean(X, 1);   %X中值的各列的均值，
X = bsxfun(@minus, X, sample_mean);
%由于X为n*d，sample_mean为1*d的，所以将sample_mean虚拟复制n行，再进行减操作
%因此得到X-sample_mean，将X进行标准化，减去均值

%%% pca projection
dim_pca = dim; %%% reduce dim if possible


% covX=zeros(dim,dim);
% sub_dim=dim/k;
% for i = 1: k
%     P=X(:,(i-1)*sub_dim+1:i*sub_dim);
%     for j=1 :k 
%         Q=X(:,(j-1)*sub_dim+1:j*sub_dim);
%     covX((i-1)*sub_dim+1:i*sub_dim,(j-1)*sub_dim+1:j*sub_dim)=P'*Q;
%     end;
% end;
% covX = covX / n;

covX= X'*X / n;
[eigVec, eigVal] = eigs(covX, dim_pca, 'LM');   %返回dim_pca个绝对值最大的特征值
%eigVec特征向量，eigVal对应的特征值对角矩阵
eigVal = diag(eigVal); 

%%% re-order the eigenvalues
dim_ordered = balanced_partition(eigVal, M);

%%% re-order the eigenvectors
R = eigVec(:, dim_ordered);

return;