function get_anchor(data_path,anchor_path,m)
dataset = fvecs_read(data_path);
dataset=dataset';
n=size(dataset,1);
p=2000;
idx=randsample(n,p);
Xtraining = dataset(idx,:);
tic;
opts = statset('Display','off','MaxIter',30);
[idc, anchor] = kmeans(Xtraining, m, 'Options', opts, 'EmptyAction', 'singleton');
save([anchor_path '.mat'],'anchor');
anchor_time=toc;

%file = fopen('anchor_time.txt', 'a+');
%fprintf(file,'%f #%s \n',anchor_time,data_name);
%fclose(file);

end
