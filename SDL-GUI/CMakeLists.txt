cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

project(DisplayGame)

add_subdirectory(external/SDL-release-2.30.5)
add_subdirectory(external/SDL_image-release-2.8.2)

add_library(SDLPlatform sdl.cpp sdl.hpp)
add_executable(DisplayGame main.cpp)

target_link_libraries(DisplayGame PRIVATE 
  SDL2
  SDL2_image
  SDLPlatform
)

target_link_libraries(SDLPlatform PRIVATE 
  SDL2
  SDL2_image
)

if (WIN32)
  target_link_libraries(
    DisplayGame PRIVATE SDL2main
  )
endif()

add_custom_command(
  TARGET DisplayGame POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2>"
    "$<TARGET_FILE:SDL2_image>" 
    "$<TARGET_FILE_DIR:DisplayGame>"
  VERBATIM
)