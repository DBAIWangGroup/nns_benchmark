%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Neighbor-Sensitive Hashing (NSH)
%     Yongjoo Park, Michael Cafarella, and Barzan Mozafari. In PVLDB Vol 9(3) 2015
%     pyongjoo@umich.edu
%     http://www-personal.umich.edu/~pyongjoo/
%
% Main training module of Neighbor-Sensitive Hashing. For simplicity of running
% demo, we are using a few heuristics. For better performance, one may want to
% tune those parameters.
%
% param 1: number of pivots (m), currently this is set to 4 * codesize
% param 2: neighbor parameter (epsilon), currently this is set to 1.9 times the
%          average distance between a pivot and its another closest pivot.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [model,pivots,W,epsilon] = trainNSH(X, codesize)
% @param X          a data matrix (row is instance and column is feature)
% @param codesize   is the hascode length of generated binary hashcodes.
% @ret   model      is a model that contains a hash function that transforms arbitrary data
%                   matrix into a hashcode matrix.

    b = codesize;
    m = 4 * codesize;           % number of pivots (chosen heuristically)
    [N d] = size(X);

    % Obtain pivots by k-means
    warning off;
    kmeans_sample_size = min(N, 10000); %randomly selct 50000 samples to do kmeans 
    [idx pivots] = kmeans(X(randperm(N, kmeans_sample_size),:), m, 'MaxIter', 10, 'Display', 'off');
    warning on;


    % Compute neighbor parameter
    anchor_dist = distMat(pivots);
    first_dist = zeros(size(anchor_dist,1),1);

    for i = 1:size(anchor_dist,1)
        [B,Idx] = sort(anchor_dist(i,:));
        min_idx = Idx(1:2);
        %min_idx = mink(anchor_dist(i,:), 2);
        first_dist(i) = anchor_dist(i,min_idx(2));   % the distance from the closest pivot
    end
    epsilon = mean(first_dist) * 1.9;       % 1.9 is chosen heuristically (other numbers also works too though)


    % Transform all data using multiple pivots
    KK = exp(-distMat(X, pivots).^2 / epsilon^2);
    KK = [KK ones(size(KK,1), 1)]; % the projection respect to each pivot , last column is 1


    % This peforms orthogonal projections to make different bits learn different
    % hash functions
    kd = size(KK, 2);       % dimension in transformed space m+1

    W = randn(kd, b);       % hyperplane in transformed space %b random hyperplane
    for i = 1:b
        W(:,i) = W(:,i) / norm(W(:,i));
    end

    V = zeros(kd, b+1);
    v = KK' * ones(N,1);  %(m+1)*1
    v = v / norm(v);
    V(:,1) = v;
    for i = 1:b
        w = W(:,i);     % [kd 1]
        w = w - V(:,1:i) * V(:,1:i)' * w;   % [kd b+1] * [b+1 kd] * [kd 1]
        W(:,i) = w / norm(w);

        a = KK * w;
        a(a>0) = 1;
        a(a<=0) = -1;

        if i ~= b
            v = KK' * a;    % [kd N] * [N 1] = [kd 1] -- O(kd * N)
            v = v - V * V' * v; % [kd b+1] * [b+1 kd] * [kd 1]
            V(:,i+1) = v / norm(v);
        end
    end

    model.hash = generateHashFunc(pivots, W, epsilon);
    
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function hash = generateHashFunc(pivots, W, epsilon)

    function QB = hash_func(Q)
        QK = exp(-distMat(Q, pivots).^2 / epsilon^2);
        QK = [QK ones(size(QK,1), 1)];
        HH = QK * W;
        HH(HH>0) = 1;
        HH(HH<=0) = 0;
        QB = compactbit(HH);
    end

    hash = @hash_func;
end
