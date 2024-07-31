Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(da_tests test_da.cpp)
add_executable(em_tests test_em.cpp)
add_executable(em_tests_math test_em_math.cpp)
add_executable(em_tests_move test_em_move.cpp)

target_link_libraries(da_tests PRIVATE Disassembler Catch2::Catch2WithMain)
target_link_libraries(em_tests PRIVATE Emulator Catch2::Catch2WithMain)
target_link_libraries(em_tests_math PRIVATE Emulator Catch2::Catch2WithMain)
target_link_libraries(em_tests_move PRIVATE Emulator Catch2::Catch2WithMain)

# automatic discovery of unit tests
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(da_tests
  PROPERTIES
    LABELS "unit"
  )
catch_discover_tests(em_tests
  PROPERTIES
    LABELS "unit"
  )
catch_discover_tests(em_tests_math
  PROPERTIES
    LABELS "unit"
  )

catch_discover_tests(em_tests_move
  PROPERTIES
    LABELS "unit"
  )