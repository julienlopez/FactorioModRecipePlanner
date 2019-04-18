#!/bin/bash

CC=gcc-7 && CXX=g++-7

mkdir build
cd build
cmake ..
make
