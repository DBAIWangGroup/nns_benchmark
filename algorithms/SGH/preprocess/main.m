function main_c(path_name, data_name)
disp(data_name)
XTrain = fvecs_read(['~/Downloads/Dataset/' path_name '/data/' data_name '_base.fvecs']);
XTrain = double(XTrain');

bits = [8, 16,32,64,96,128];%,256
nb = size(bits,2);

%SGH(XTrain,XTest,bit(i),Xgnd,fid_precision,fid_recall,fid_reranking);

n = size(XTrain,1);

tic;

m = 300;
sample = randperm(n);
center = XTrain(sample(1:m),:);
KTrain = distMat(XTrain,center);
KTrain = KTrain.*KTrain;
delta = mean(mean(KTrain,2));
KTrain = exp(-KTrain/(2*delta));

bias = mean(KTrain);
KXTrain = KTrain-repmat(bias,n,1);
    
%% Construct PX and QX
FnormX = sum(XTrain.*XTrain,2);
rho = 2;
FnormX = exp(-FnormX/rho);
alpha = sqrt(2*(exp(1)-exp(-1))/rho);
part = bsxfun(@times,alpha*XTrain,FnormX);
%PX = [part,sqrt(exp(1)+exp(-1))*FnormX, 1*ones(n,1)];
clear XTrain;

PX=zeros(size(part,1),size(part,2)+2);
QX = zeros(size(part,1),size(part,2)+2);

PX(:,1:size(part,2)) = part;
tmp=sqrt(exp(1)+exp(-1))*FnormX;
PX(:,size(part,2)+1)=tmp;
PX(:,size(part,2)+2)=1*ones(n,1);


QX(:,1:size(part,2))= part;
QX(:,size(part,2)+1)=tmp;
QX(:,size(part,2)+2)=-1*ones(n,1);
clear FnormX part tmp;
prepare_time=toc;

for i = 1:nb
    %% Construct Kernel
    % construct KXTrain
    tic;
    

    %% Training
    % Sequential learning algorithm to learn Wx
    [Wx] = trainSGH(KXTrain,PX,QX,bits(i));
    XX = KXTrain*Wx;
    X=zeros(size(XX));
    X(XX>=0)=1;
    B1=compactbit(X>0);
    clear X;
    index_time= toc;
    
    file = fopen(['indices/' data_name '_' num2str(bits(i)) '.txt'], 'wb');
    fprintf(file,'%d\n',bits(i));
    fprintf(file,'%f\n',delta);
    
    for j=1:m %center
        p_=center(j,:);
        fprintf(file,'%f ',p_);
        fprintf(file,'\n');
    end
    fprintf(file,'%f ',bias);
    fprintf(file,'\n');
    for j=1:m
        m_=Wx(j,:);
        fprintf(file,'%f ',m_);
        fprintf(file,'\n');
    end
    fclose(file);
    
    %%%%%%%%%%%%%%%%%%%%
    % write B1
    ivecs_write(['B1/' data_name '_' num2str(bits(i)) '.ivecs'],B1');
    
    %write index_time
    file = fopen('index.txt', 'a+');
    fprintf(file,'%f #%s_%dbits \n',index_time+prepare_time,data_name,bits(i));
    fclose(file);

end
end
