function recall = computeRecall(DB, G, r)
% return the recall of k-NN retrieval
% 
% Input
%   DB: distance matrix calculated from the hashed matrix
%   r: number of the documents retrieved 
%
%   The dimensions of DX and DB are [#query, #data].

[qn xn] = size(DB);
assert(r <= xn);

k = size(G,2);
recall = zeros(qn, 1);

parfor i = 1:qn
    %qid = i;

    %groundtruth_dist = distMat(q, X(G(qid,:),:));
    %min_index = mink(gold_standard_dist, k);
    groundtruth = G(i, :);
    [B,I] = sort(double(DB(i,:)));
    hash_neighbor = I(1:r);
    %hash_neighbor = mink(double(DB(i,:)), r);

    recall(i) = length(intersect(groundtruth, hash_neighbor)) / k;
end

recall = mean(recall);

