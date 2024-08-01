#!/bin/bash

mkdir build
cmake -S . -B build
cmake --build build
ctest --test-dir ./build --output-on-failure