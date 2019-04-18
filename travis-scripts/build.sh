#!/bin/bash

CC=gcc-8 && CXX=g++-8

mkdir build
cd build
cmake ..
make VERBOSE=1
