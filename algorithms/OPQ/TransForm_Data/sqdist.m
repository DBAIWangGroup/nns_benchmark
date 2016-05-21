function d=sqdist(a,b)
% SQDIST - computes squared Euclidean distance matrix
%          computes a rectangular matrix of pairwise distances  计算A和B中点的成对距离
% between points in A (given in columns) and points in B

% NB: very fast implementation taken from Roland Bunschoten

%aa = sum(a.*a,1); a中各数据的长度，  bb = sum(b.*b,1); ab = a'*b;   a和b的内积
%d = abs(repmat(aa',[1 size(bb,2)]) + repmat(bb,[size(aa,2) 1]) - 2*ab);

%aa = sum(a.*a,1)'; bb = sum(b.*b,1);
%d = abs(repmat(sum(a.*a,1)',[1 size(b,2)]) + repmat(sum(b.*b,1),[size(a,2) 1]) - 2*a'*b);

d = bsxfun(@plus, -2*a'*b, sum(b.*b,1));
d = abs(bsxfun(@plus, d', sum(a.*a,1)))';