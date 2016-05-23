function main_c_OneLayer(data_path, anchor_path, params_path,B1_path,s,bit)

dataset = fvecs_read(data_path); 
dataset=dataset'; % dataset = [n,d] 

[n,dim] = size(dataset);

anchor=load([anchor_path '.mat']);
anchor=anchor.anchor;
m=size(anchor,1);

tic;
[Y, W, sigma] = OneLayerAGH_Train(dataset, anchor, bit, s, 0);
B1 = compactbit(Y);
index_time=toc;
    
file = fopen(params_path, 'w');
fprintf(file,'%d\n',bit);
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
fclose(file);  
ivecs_write(B1_path,B1');

end
