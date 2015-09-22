export INCLUDE=include
export LIB=/usr/lib

mkdir files
rm -r build
mkdir build
cd build
cmake -Dtest=ON ../
make

cd ..
./build/unitTest
./build/udrive
