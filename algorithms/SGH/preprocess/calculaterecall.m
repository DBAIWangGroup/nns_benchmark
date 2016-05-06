% This function computes Top-k precision. If there is no extra space to
% store variable Dhamm = hammingDist(BXTest,BXTrain) and Wtrue =
% genGroundTruth(XTest,XTrain), you can use this function to compute Top-k
% precision.
% But this function is slower than callTopk();
function [Topk search_time] = calculaterecall(BXTrain,BXTest,XTrain,XTest,k,gnd)
nt = size(XTest,1);
[Ntrain,~] = size(XTrain);

ap = zeros(nt,1);
result=zeros(nt,k);
t1=cputime;
for i = 1:nt
    ham = hammingDist(BXTest(i,:), BXTrain);
    [~,index] = sort(ham);
    index=index(1:k);
    result(i,:)=index;
end
search_time=(cputime-t1)/nt;

for i=1:nt
    ids=gnd(i,:)+1;
    ap(i) = length(intersect(ids,result(i,:)))/k;
end;
Topk = mean(ap);