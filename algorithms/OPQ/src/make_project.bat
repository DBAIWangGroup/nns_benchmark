cd build
del CMakeCache.txt
cmake -DMAKE_ONLY=BUILD_ALL  -G "Visual Studio 10 Win64"  ..
pause
