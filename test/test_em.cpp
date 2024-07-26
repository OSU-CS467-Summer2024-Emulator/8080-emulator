// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>
#include "emulator/emulator.hpp"

bool operator==(const Registers &lhs, const Registers &rhs)
{
    return lhs.A == rhs.A && lhs.B == rhs.B && lhs.C == rhs.C && lhs.D == rhs.D &&
           lhs.E == rhs.E && lhs.H == rhs.H && lhs.L == rhs.L;
}

bool operator==(const Flags &lhs, const Flags &rhs)
{
    return lhs.z == rhs.z && lhs.s == rhs.s && lhs.p == rhs.p &&
           lhs.cy == rhs.cy && lhs.ac == rhs.ac;
}

TEST_CASE("A simple test", "[fast]")
{
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("parity", "[flag]")
{
    Emulator e;
    REQUIRE(e.parity(0x00, 8) == 1);
    REQUIRE(e.parity(0x01, 8) == 0);
    REQUIRE(e.parity(0xf1, 8) == 0);
    REQUIRE(e.parity(0xff, 8) == 1);
}

TEST_CASE("0x00 NOP", "[opcode]")
{
    Emulator e;
    Registers registers_before = e.GetRegisters();
    Flags flags_before = e.GetFlags();
    int pc_before = e.GetPC();
    int sp_before = e.GetSP();

    e.EmulateOpcode(0x00);

    REQUIRE(e.GetRegisters() == registers_before);
    REQUIRE(e.GetFlags() == flags_before);
    REQUIRE(e.GetPC() == pc_before + 1);
    REQUIRE(e.GetSP() == sp_before);
}

// Random other example
TEST_CASE("0x04 INR B", "[opcode]")
{
    Emulator e;
    Registers registers_before = e.GetRegisters();
    Flags flags_before = e.GetFlags();
    int pc_before = e.GetPC();
    int sp_before = e.GetSP();

    e.EmulateOpcode(0x04);

    Registers registers_after = registers_before;
    registers_after.B++;

    Flags flags_after;
    flags_after.p = 1;

    REQUIRE(e.GetRegisters() == registers_after);
    REQUIRE(e.GetFlags() == flags_before);
    REQUIRE(e.GetPC() == pc_before + 1);
    REQUIRE(e.GetSP() == sp_before);
}
