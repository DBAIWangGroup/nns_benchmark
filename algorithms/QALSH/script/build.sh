data_path=../../../data
index_path=../index
mkdir ${index_path}

for data in "audio"
do
	index_path="${index_path}/${data}"
	mkdir ${index_path}

	n=53387
	qn=200
	d=192
	B=4096
	c=2

	cd ../src

	echo "== Indexing... =="
	./qalsh -alg 1 -n $n -d $d -B ${B} -c $c -ds ${data_path} -of ${index_path}/c=$c -if ${index_path}/index.txt

done