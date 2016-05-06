function main_c_TwoLayer(path_name, data_name,s)

traindata = fvecs_read(['~/Downloads/Dataset/' path_name '/data/' data_name '_base.fvecs']);
traindata=traindata';

[n,dim] = size(traindata);
% nq = size(testdata,1);
m = 300; %number of anchors, I simply fix this parameter 
bits = [8,16,32,64,96,128]; %number of hash bits ,256

%% Please run K-means clustering to get m anchor points 

anchor=load(['anchor/' data_name '_anchor_300.mat']);
anchor=anchor.anchor;
%% One-Layer AGH
for i = 1:length(bits)
    
    n=size(traindata,1);
    
    tic;
    [Y, W, thres, sigma] = TwoLayerAGH_Train(traindata, anchor, bits(i), s, 0);
    B1 = compactbit(Y);
    index_time=toc;
    
    file = fopen(['indices_TwoLayer/' data_name '_' num2str(bits(i)) '_' num2str(s) '.txt'], 'w');
    fprintf(file,'%d\n',bits(i));
    fprintf(file,'%f\n',sigma);
    for t=1:m %anchor
        p_=anchor(t,:);
        fprintf(file,'%f ',p_);
        fprintf(file,'\n');
    end
    for t=1:m %W
        p_=W(t,:);
        fprintf(file,'%f ',p_);
        fprintf(file,'\n');
    end
    
    for t=1:2 %Thres
        p_=thres(t,:);
        fprintf(file,'%f ',p_);
        fprintf(file,'\n');
    end
    fclose(file);
    
    ivecs_write(['B1_TwoLayer/' data_name '_' num2str(bits(i)) '_' num2str(s) '.ivecs'],B1');
    
    %write index_time
    file = fopen('index_TwoLayer.txt', 'a+');
    fprintf(file,'%f #%s_%dbits %ds \n',index_time,data_name,bits(i),s);
    fclose(file);
    
end
end
