data_path=../../../data
query_path=../../../data
gnd_path=../../../data
index_path=../index
result_path=../results
mkdir ${result_path}

for data in "audio"
do
	index_path="${index_path}/${data}"

	qn=200
	d=192
	B=4096
	k=20
	c=2

	cd ../src

	./qalsh -alg 2 -qn $qn -d $d -qs ${query_path} -ts ${gnd} -of ${index_path} -rf ${result_path} -B ${B} -c $c

done
