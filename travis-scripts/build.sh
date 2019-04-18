#!/bin/bash

CC=gcc-9 && CXX=g++-9

mkdir build
cd build
cmake ..
make VERBOSE=1
