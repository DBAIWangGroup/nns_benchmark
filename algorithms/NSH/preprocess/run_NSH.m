function run_NSH(data_path, params_path, binary_path, codesize)

dataset = fvecs_read(data_path);
X=dataset';
clear dataset;

tic;
[model,pivots,W,eps] = trainNSH(X, codesize);
XB = model.hash(X);
index_time=toc;

m=size(pivots,1);
file = fopen(params_path, 'w');
fprintf(file,'%d\n',codesize);
fprintf(file,'%f\n',eps);
for t=1:m %anchor
    p_=pivots(t,:);
    fprintf(file,'%f ',p_);
    fprintf(file,'\n');
end
for t=1:m+1 %W
    p_=W(t,:);
    fprintf(file,'%f ',p_);
    fprintf(file,'\n');
end
fclose(file);
    
ivecs_write(binary_path ,XB');
  
disp(['indextime: ' num2str(index_time) ' (s)']);
   
end
