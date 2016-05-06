function dim_ordered = balanced_partition(vals, M)

% Devide vals into M "balanced" subspaces


dim = numel(vals);          %数组中元素个数
dim_subspace = dim / M;

dim_tables = cell(M, 1);    %创建M*1的cell元胞数组，

fvals = log(vals+1e-20); %% balance the product of eigenvalues of the subspaces, i.e., the sum of log(eigenvalues)
fvals = fvals - min(fvals) + 1; %% make all positive

sum_list = zeros(M, 1);

current_subspaceIdx = 1;

for d=1:dim
    
    dim_tables{current_subspaceIdx} = [dim_tables{current_subspaceIdx}; d];
    
    sum_list(current_subspaceIdx) = sum_list(current_subspaceIdx) + fvals(d);
    
    if numel(dim_tables{current_subspaceIdx}) == dim_subspace %% this subspace is full
        sum_list(current_subspaceIdx) = 1e10;  %% do not use
    end
    
    [not_used, current_subspaceIdx] = min(sum_list);
end

dim_ordered = [];
for m=1:M
    %disp(dim_tables{m});
    dim_ordered = [dim_ordered; dim_tables{m}];
end

end