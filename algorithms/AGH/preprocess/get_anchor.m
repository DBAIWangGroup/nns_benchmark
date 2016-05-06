
function get_anchor(path_name,data_name)
dataset = fvecs_read(['/home/yzhang4/HD/home/liwen/Downloads/Dataset/' path_name '/data/' data_name '_base.fvecs']);
dataset=dataset';
n=size(dataset,1);
p=2000;
idx=randsample(n,p);
Xtraining = dataset(idx,:);
tic;
opts = statset('Display','off','MaxIter',50);
[idc, anchor] = kmeans(Xtraining, 300, 'Options', opts, 'EmptyAction', 'singleton');
save(['anchor/' data_name '_anchor_300.mat'],'anchor');

anchor_time=toc;
file = fopen('anchor_time.txt', 'a+');
fprintf(file,'%f #%s \n',anchor_time,data_name);
fclose(file);
    

end