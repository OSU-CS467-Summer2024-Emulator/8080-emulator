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

TEST_CASE("Flag functions", "[flag]")
{
    Emulator e;
    Flags f = {};
    SECTION("Parity")
    {
        CHECK(e.parity(0x00, 8) == 1);
        CHECK(e.parity(0x01, 8) == 0);
        CHECK(e.parity(0xf1, 8) == 0);
        CHECK(e.parity(0xff, 8) == 1);
    }
    SECTION("LogicFlags")
    {
        // move value 0x00 into A
        e.EmulateOpcode(0x3e, 0x00);
        REQUIRE(e.GetRegisters().A == 0x00);
        e.LogicFlagsA();
        f = {.z = 1, .s = 0, .p = 1, .cy = 0};
        CHECK(e.GetFlags() == f);

        // move value 0xab into A
        e.EmulateOpcode(0x3e, 0xab);
        REQUIRE(e.GetRegisters().A == 0xab);
        e.LogicFlagsA();
        f = {.z = 0, .s = 1, .p = 0, .cy = 0};
        CHECK(e.GetFlags() == f);

        // move value 0x11 into A
        e.EmulateOpcode(0x3e, 0x11);
        REQUIRE(e.GetRegisters().A == 0x11);
        e.LogicFlagsA();
        f = {.z = 0, .s = 0, .p = 1, .cy = 0};
        CHECK(e.GetFlags() == f);
    }
    SECTION("Arithmetic flags")
    {
        uint16_t num = 0x0000;
        e.ArithFlagsA(num);
        f = {.z = 1, .s = 0, .p = 1, .cy = 0};
        CHECK(e.GetFlags() == f);

        num = 0x0123;
        e.ArithFlagsA(num);
        f = {.z = 0, .s = 0, .p = 0, .cy = 1};
        CHECK(e.GetFlags() == f);

        num = 0x00dd;
        e.ArithFlagsA(num);
        f = {.z = 0, .s = 1, .p = 1, .cy = 0};
        CHECK(e.GetFlags() == f);
    }
    SECTION("ZSP flags")
    {
        uint8_t num = 0x0000;
        e.ZSPFlags(num);
        f = {.z = 1, .s = 0, .p = 1, .cy = 0};
        CHECK(e.GetFlags() == f);

        num = 0x23;
        e.ZSPFlags(num);
        f = {.z = 0, .s = 0, .p = 0, .cy = 0};
        CHECK(e.GetFlags() == f);

        num = 0xdd;
        e.ZSPFlags(num);
        f = {.z = 0, .s = 1, .p = 1, .cy = 0};
        CHECK(e.GetFlags() == f);

        // set carry
        e.ArithFlagsA(0x0100);
        REQUIRE(e.GetFlags().cy == 1);
        num = 0x23;
        e.ZSPFlags(num);
        f = {.z = 0, .s = 0, .p = 0, .cy = 1};
        CHECK(e.GetFlags() == f);
    }
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

TEST_CASE("Move Immediate", "[opcode][move][immediate]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("MVI B")
    {
        e.EmulateOpcode(0x06, 0xbb);
        CHECK((uint8_t)e.GetRegisters().B == 0xbb);
        CHECK(e.GetPC() == pc + 2);
    }
    SECTION("MVI C")
    {
        e.EmulateOpcode(0x0e, 0xcc);
        CHECK((uint8_t)e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 2);
    }
    SECTION("MVI D")
    {
        e.EmulateOpcode(0x16, 0xdd);
        CHECK((uint8_t)e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 2);
    }
    SECTION("MVI E")
    {
        e.EmulateOpcode(0x1e, 0xee);
        CHECK((uint8_t)e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 2);
    }
    SECTION("MVI H")
    {
        e.EmulateOpcode(0x26, 0x11);
        CHECK((uint8_t)e.GetRegisters().H == 0x11);
        CHECK(e.GetPC() == pc + 2);
    }
    SECTION("MVI L")
    {
        e.EmulateOpcode(0x2e, 0x22);
        CHECK((uint8_t)e.GetRegisters().L == 0x22);
        CHECK(e.GetPC() == pc + 2);
    }
    SECTION("MVI M")
    {
        e.AllocateMemory(0x3000);

        // Write address to HL registers
        e.EmulateOpcode(0x26, 0x25);
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE((uint8_t)e.GetRegisters().H == 0x25);
        REQUIRE((uint8_t)e.GetRegisters().L == 0x00);

        // move 0x33 to memory at (HL)
        e.EmulateOpcode(0x36, 0x33);

        CHECK(e.ReadFromMem(0x2500) == 0x33);
    }
    SECTION("MVI A")
    {
        e.EmulateOpcode(0x3e, 0xaa);
        CHECK((uint8_t)e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 2);
    }
}

TEST_CASE("Subtraction", "[math]")
{
    Emulator e;

    // aa - aa = 0
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);
    e.SubtractFromA(0xaa);
    CHECK(e.GetRegisters().A == 0x00);
    Flags result = {.z = 1, .s = 0, .p = 1, .cy = 0};
    CHECK(e.GetFlags() == result);

    // 20 - 10 = 10
    e.EmulateOpcode(0x3e, 0x20);
    REQUIRE(e.GetRegisters().A == 0x20);
    e.SubtractFromA(0x10);
    CHECK(e.GetRegisters().A == 0x10);
    result = {.z = 0, .s = 0, .p = 0, .cy = 0};
    CHECK(e.GetFlags() == result);

    // aa - c5 = e5
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);
    e.SubtractFromA(0xc5);
    CHECK(e.GetRegisters().A == 0xe5);
    result = {.z = 0, .s = 1, .p = 0, .cy = 1};
    CHECK(e.GetFlags() == result);
}

TEST_CASE("Addition", "[opcode][add][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("ADD B")
    {
        int test_pc = pc;

        // MVI B
        e.EmulateOpcode(0x06, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.GetRegisters().B == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD B
        e.EmulateOpcode(0x80);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetRegisters().B == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD C")
    {
        int test_pc = pc;

        // MVI C
        e.EmulateOpcode(0x0e, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.GetRegisters().C == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD C
        e.EmulateOpcode(0x81);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetRegisters().C == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD D")
    {
        int test_pc = pc;

        // MVI D
        e.EmulateOpcode(0x16, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.GetRegisters().D == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD D
        e.EmulateOpcode(0x82);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetRegisters().D == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD E")
    {
        int test_pc = pc;

        // MVI D
        e.EmulateOpcode(0x1e, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.GetRegisters().E == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD D
        e.EmulateOpcode(0x83);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetRegisters().E == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD H")
    {
        int test_pc = pc;

        // MVI H
        e.EmulateOpcode(0x26, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.GetRegisters().H == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD H
        e.EmulateOpcode(0x84);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetRegisters().H == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD L")
    {
        int test_pc = pc;

        // MVI L
        e.EmulateOpcode(0x2e, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.GetRegisters().L == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD L
        e.EmulateOpcode(0x85);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetRegisters().L == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD M")
    {
        int test_pc = pc;
        e.AllocateMemory(0x3000);

        // Write address to HL registers
        // MVI H, 0x25
        // MVI L, 0x00
        e.EmulateOpcode(0x26, 0x25);
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE((uint8_t)e.GetRegisters().H == 0x25);
        REQUIRE((uint8_t)e.GetRegisters().L == 0x00);
        test_pc = e.GetPC();

        // MVI M, 0x30
        e.EmulateOpcode(0x36, 0x30);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.ReadFromMem(0x2500) == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD M
        e.EmulateOpcode(0x86);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADD A")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x30);
        REQUIRE(e.GetRegisters().A == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD A
        e.EmulateOpcode(0x87);
        CHECK(e.GetRegisters().A == 0x60);
        CHECK(e.GetPC() == test_pc + 1);
    }
}
