#!/bin/bash

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
sudo apt-get update
sudo apt-get install -y cmake gcc-8 g++-8 lcov

sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-8 90
