rm raytrace
rm Makefile
rm -r CMakeFiles
rm CMakeCache.txt
rm cmake_install.cmake
cmake .
make
./raytrace ./data/scene.xml result

