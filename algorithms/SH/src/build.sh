rm *.o 
rm SH


for f in *.cpp 
do
g++ -O3 -c *.cpp 
done

g++ -o SH -O3 *.o 
rm *.o 

