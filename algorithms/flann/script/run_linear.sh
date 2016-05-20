data_path=../../../data
gnd_path=../results
mkdir ${gnd_path}
k=20

for data in "audio"
do

../code/build/bin/linear -f ${data_path}/${data}.hdf5 -g ${gnd_path}/${data}_gnd.hdf5 -k ${k}

done

