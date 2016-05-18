
make kgraph_index 
make kgraph_search
make fvec2lshkit 
make lshkit2fvec

make clean

cp fvec2lshkit lshkit2fvec ../../data

path=`pwd`

cd ../../data

for data in audio sift
do 

fvec2lshkit ${data}_base.fvecs ${data}_base.lshkit

fvec2lshkit ${data}_query.fvecs ${data}_query.lshkit

fvec2lshkit ${data}_groundtruth.ivecs ${data}_groundtruth.lshkit 

done

cd $path


