#!/bin/bash

mkdir build
cmake -S . -B build
cmake --build build
./build/SDL-GUI/Main