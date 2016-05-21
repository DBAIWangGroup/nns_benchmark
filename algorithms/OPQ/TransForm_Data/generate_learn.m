function generate_learn(data_path,learn_path,m)

dataset=fvecs_read(data_path);
n=size(dataset,2);
idx=randsample(n,m);
learn=dataset(:,idx);

fvecs_write(learn_path,learn);
end
