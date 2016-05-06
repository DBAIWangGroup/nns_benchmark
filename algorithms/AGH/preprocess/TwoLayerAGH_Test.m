%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Y = TwoLayerAGH_Test(X, Anchor, W, Thres, s, sigma)
%
% Two-Layer Anchor Graph Hashing Test 
% Written by Wei Liu (wliu@ee.columbia.edu)
% X = nXdim input data 
% Anchor = mXdim anchor points (m<<n)
% W = mX(r/2) projection matrix in spectral space
% Thres = 2X(r/2) threshold matrix
% s = number of nearest anchors
% sigma: Gaussian RBF kernel width parameter 
% Y = nXr binary codes (Y_ij in {1,0})
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[n,dim] = size(X);
m = size(Anchor,1);
r1 = size(W,2);


%% get Z
Z = zeros(n,m);
Dis = sqdist(X',Anchor');
clear X;
clear Anchor;

val = zeros(n,s);
pos = val;
for i = 1:s
    [val(:,i),pos(:,i)] = min(Dis,[],2);
    tep = (pos(:,i)-1)*n+[1:n]';
    Dis(tep) = 1e60; 
end
clear Dis;
clear tep;
val = exp(-val/(1/1*sigma^2));
val = repmat(sum(val,2).^-1,1,s).*val; %% normalize
tep = (pos-1)*n+repmat([1:n]',1,s);
Z([tep]) = [val];
Z = sparse(Z);
clear tep;
clear val;
clear pos;


%% get binary codes
Y = Z*W;
Y1 = Y; 
for k = 1:r1
    pos = find(Y1(:,k)>0);
    neg = setdiff([1:n],pos);
    Y1(pos,k) = Y1(pos,k)-Thres(1,k);
    Y1(neg,k) = -Y1(neg,k)+Thres(2,k);
end
Y = [Y,Y1];

Y = (Y>0); %% logical format
