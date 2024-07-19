#!/bin/bash

mkdir build
cmake -S . -B build
cmake --build build
#./build/disassembler/Disassembler ./space_invaders_rom/invaders
./build/emulator/Emulator