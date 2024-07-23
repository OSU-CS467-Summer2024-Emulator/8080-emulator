add_library(Emulator emulator.cpp emulator.hpp)
add_executable(Main main.cpp)
target_link_libraries(Main Emulator Disassembler) 