#!/bin/bash

mkdir build
cmake -S . -B build
cmake --build build
./build/emulator/Main