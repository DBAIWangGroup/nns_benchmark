function generate_learn(file_path,data_path,m)

mkdir(['~/Downloads/Dataset/' file_path '/learn']);

dataset=fvecs_read(['~/Downloads/Dataset/' file_path '/data/' data_path '_base.fvecs']);

n=size(dataset,2);
idx=randsample(n,m);
learn=dataset(:,idx);

fvecs_write(['~/Downloads/Dataset/' file_path '/learn/' data_path '_learn_' num2str(m) '.fvecs'],learn);
end
