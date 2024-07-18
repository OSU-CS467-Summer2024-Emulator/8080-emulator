add_library(Emulator emulator.cpp)
add_executable(Main main.cpp)
target_link_libraries(Main Emulator) 