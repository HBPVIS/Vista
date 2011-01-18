#!/bin/bash

cd ../..

# Do a make clean before cleaning up, because
# after the clean you cannot use make :-)

make clean

# Remove ugly stuff created by CMake

find . -name CMakeFiles -print0 | xargs -0 rm -r
find . -name Makefile -delete
find . -name cmake_install.cmake -delete
find . -name CMakeCache.txt -delete

# If you need any uninstalling,
# you should keep the following file

rm install_manifest.txt
rm cmake_uninstall.cmake
