// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>
#include "emulator/emulator.hpp"

TEST_CASE("A simple test", "[fast]")
{
    REQUIRE(1 + 1 == 2);
}
