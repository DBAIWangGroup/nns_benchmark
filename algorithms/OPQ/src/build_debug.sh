mkdir -p build_dup
cd build_dup
rm ./CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Debug ..
make