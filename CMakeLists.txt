cmake_minimum_required(VERSION 3.10)

project(Disassembler)  # Project name

set (CMAKE_CXX_STANDARD 11) # Allow C++11 extensions
SET(BASEPATH "${CMAKE_SOURCE_DIR}")
include_directories(${BASEPATH})

enable_testing()

# Add subdirectories to the project
add_subdirectory(disassembler) 
add_subdirectory(emulator)
add_subdirectory(test)