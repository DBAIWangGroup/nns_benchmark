function [Wtrue] = genGroundTruth(XTest,XTrain)
num_test = size(XTest,1);
num_training = size(XTrain,1);
% rate = 2%
avrageNumberNeighbor = floor(num_training*0.02);

Wtrue = zeros(num_test,num_training);
dist = distMat(XTest,XTrain);
[sorted,~] = sort(dist,2);
threshold = sorted(:,avrageNumberNeighbor);
Wtrue = bsxfun(@ge,threshold,dist);

end
