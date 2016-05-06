function map = calculateMap(BXTrain,BXTest,XTrain,XTest)
[Ntest,~] = size(XTest);
[Ntrain,~] = size(XTrain);
ap = zeros(1,Ntest);

averageNumberNeighbor = floor(Ntrain*0.02);

for j = 1:Ntest
    dist = distMat(XTest(j,:),XTrain);
    [sorted,~] = sort(dist,2);
    threshold = sorted(:,averageNumberNeighbor);
    gnd = bsxfun(@ge,threshold,dist);
    tsum = sum(gnd);
    if tsum == 0
        continue;
    end
    ham = hammingDist(BXTest(j,:), BXTrain);
    [~,index] = sort(ham);
    gnd = gnd(index);
    count = 1:tsum;
    tindex = find(gnd == 1);
    ap(j) = mean(count./tindex);

end

map = mean(ap,2);
end

