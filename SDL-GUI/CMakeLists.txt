add_subdirectory(external/SDL-release-2.30.5)

add_library(SDLPlatform sdl.cpp sdl.hpp sound.cpp sound.hpp)
add_executable(Main main.cpp)

target_link_libraries(Main PRIVATE 
  SDL2
  SDLPlatform
  Emulator
)

target_link_libraries(SDLPlatform PRIVATE 
  SDL2
  Emulator
)

if (WIN32)
  target_link_libraries(
    Main PRIVATE SDL2main
  )

  target_link_libraries(
    SDLPlatform PRIVATE SDL2main
  )
endif()

add_custom_command(
  TARGET Main POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2>"
    "$<TARGET_FILE:SDLPlatform>" 
    "$<TARGET_FILE_DIR:Main>"
  VERBATIM
)