% SGH(XTrain,XTest,bit)
% SGH procedure
% Input:
%   XTrain: training data matrix(nxd), where n is number of the training data points
%   and d is the dimension of each data point.
%   XTest: testing data matrix(mxd).
%   bit: number of bit.
% Output:
%   map: MAP
%   topk: top-1k precision
function [map,topk] = SGH(XTrain,XTest,bit,Xgnd,fid_precision,fid_recall,fid_reranking)
num_training = size(XTrain,1);
num_testing = size(XTest,1);
% k = 100;

start_time = cputime;
%% Construct PX and QX
FnormX = sum(XTrain.*XTrain,2);
rho = 2;
FnormX = exp(-FnormX/rho);
alpha = sqrt(2*(exp(1)-exp(-1))/rho);
part = bsxfun(@times,alpha*XTrain,FnormX);
PX = [part,sqrt(exp(1)+exp(-1))*FnormX, 1*ones(num_training,1)];
QX = [part,sqrt(exp(1)+exp(-1))*FnormX,-1*ones(num_training,1)];
clear FnormX part;

%% Construct Kernel
% construct KXTrain
m = 300;
sample = randperm(num_training);
center = XTrain(sample(1:m),:);
KTrain = distMat(XTrain,center);
KTrain = KTrain.*KTrain;
delta = mean(mean(KTrain,2));
KTrain = exp(-KTrain/(2*delta));

bias = mean(KTrain);
KXTrain = KTrain-repmat(bias,num_training,1);


fprintf('bit:%d\n',bit);
%% Training
% Sequential learning algorithm to learn Wx
[Wx] = trainSGH(KXTrain,PX,QX,bit);
XX = KXTrain*Wx;
X=zeros(size(XX));
X(XX>=0)=1;
B1=compactbit(X>0);
clear X;
index_time= cputime-start_time;
index_time


% construct KXTest
% tic;
% KTest = distMat(XTest,center);
% KTest = KTest.*KTest;
% KTest = exp(-KTest/(2*delta));
% KXTest = KTest-repmat(bias,num_testing,1);
% YY=KXTest*Wx;
% Y=zeros(size(YY));
% Y(YY>=0)=1;
% B2=compactbit(Y>0);
% clear Y;



% file = fopen(['/home/liwen/Documents/LSH/SGH/code/indices/MNIST_' num2str(bit) '.dat'], 'w');
% fwrite(file, Wx, 'double');
% fwrite(file,center ,'double');
% fwrite(file,delta,'double');
% fwrite(file,bias,'double');
% 
% fwrite(file, B1, 'uint8');
% fclose(file);


%% Evaluation

% result=zeros(num_testing,num_training);
% for i = 1:num_testing
%     ham = hammingDist(B2(i,:), B1);
%     [~,index] = sort(ham);
%     %index=index(1:k);
%     result(i,:)=index;
% end
% search_time=toc/num_testing;
% 
% nq=num_testing;
%     tic;
%     top_K_result=result(:,1:k);
%     ext_topk_time = toc/nq;
%     
%     ap=zeros(nq,1);
%     for i=1:nq
%         ids=Xgnd(i,:)+1;
%         ap(i)=length(intersect(ids,top_K_result(i,:)))/k;
%     end
%     recall=mean(ap);
%     fprintf(fid_recall,'%.6f %.6f %.6f #bit_%d \n',recall,search_time+ext_topk_time,index_time,bit);
%     
%     %%%%%%%non-reranking recall
%     N=[1,10,100,1000,10000];
%     for rn = 1:5
%         ap=zeros(nq,1);
%         for i=1:nq
%             ids=Xgnd(i,1)+1;
%             ap(i)=find(result(i,:)==ids);
%         end
%         precision=length(find(ap<=N(rn)))/nq;
%         
%         fprintf(fid_precision,'%.6f ',precision);
%     end
%     fprintf(fid_precision,'%.6f %.6f #bit_%d \n',search_time,index_time,bit);
%     
%     %%%%%%%reranking precision
%     rerank_n=k+100;
%     tic;
%     result_reranking=zeros(nq,rerank_n);
%     for i= 1:nq
%         dist=zeros(1,rerank_n);
%         for q=1:rerank_n
%             dist(1,q)=sqdist(XTest(i,:)',XTrain(result(i,q),:)');
%         end
%         [~,ind]=sort(dist);
%         result_reranking(i,:)=result(i,ind);
%     end
%     reranking_time=toc/nq;
%     
%     ap=zeros(nq,1);
%     for i=1:nq
%        ids=Xgnd(i,:)+1;
%        ap(i)=length(intersect(ids,result_reranking(i,1:k)))/k;
%     end
%     precision=mean(ap);
%     fprintf(fid_reranking,'%.6f %.6f %.6f #bit_%d \n',precision,search_time+reranking_time,index_time,bit);   

end