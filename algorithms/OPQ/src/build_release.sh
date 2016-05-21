mkdir -p build_master
cd build_master
rm ./CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Release ..
make
