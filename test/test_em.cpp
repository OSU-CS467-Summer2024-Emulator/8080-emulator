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

TEST_CASE("Set and complement carry flag", "[opcode][flag]")
{
    Emulator e;

    SECTION("STC (set carry)")
    {
        REQUIRE(e.GetFlags().cy == 0);

        uint16_t pc_before = e.GetPC();
        e.EmulateOpcode(0x37);

        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        e.EmulateOpcode(0x37);

        REQUIRE(e.GetFlags().cy == 1);
    }
    SECTION("CMC (complement carry)")
    {
        REQUIRE(e.GetFlags().cy == 0);

        uint16_t pc_before = e.GetPC();
        e.EmulateOpcode(0x3f);

        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        e.EmulateOpcode(0x3f);

        REQUIRE(e.GetFlags().cy == 0);
    }
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

TEST_CASE("Add immediate", "[opcode][add][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("ADI D8")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x30);
        REQUIRE(e.GetRegisters().A == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADI 0x30
        e.EmulateOpcode(0xc6, 0x30);
        CHECK(e.GetRegisters().A == 0x60);
        CHECK(e.GetPC() == test_pc + 2);
    }
}

TEST_CASE("Add with carry", "[opcode][add][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("ADC B")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().B == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI B
        e.EmulateOpcode(0x06, 0x08);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD B
        // Trigger a carry operation
        e.EmulateOpcode(0x80);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC B
        e.EmulateOpcode(0x88);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC C")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().C == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI C
        e.EmulateOpcode(0x0e, 0x08);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().C == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD C
        // Trigger a carry operation
        e.EmulateOpcode(0x81);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetRegisters().C == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC C
        e.EmulateOpcode(0x89);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetRegisters().C == 0x08);
        REQUIRE(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC D")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().D == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI D
        e.EmulateOpcode(0x16, 0x08);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().D == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD D
        // Trigger a carry operation
        e.EmulateOpcode(0x82);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetRegisters().D == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC D
        e.EmulateOpcode(0x8a);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetRegisters().D == 0x08);
        REQUIRE(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC E")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().E == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI E
        e.EmulateOpcode(0x1e, 0x08);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().E == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD E
        // Trigger a carry operation
        e.EmulateOpcode(0x83);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetRegisters().E == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC E
        e.EmulateOpcode(0x8b);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetRegisters().E == 0x08);
        REQUIRE(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC H")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().H == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI H
        e.EmulateOpcode(0x26, 0x08);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().H == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD H
        // Trigger a carry operation
        e.EmulateOpcode(0x84);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetRegisters().H == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC H
        e.EmulateOpcode(0x8c);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetRegisters().H == 0x08);
        REQUIRE(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC L")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().L == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x08);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetRegisters().L == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD L
        // Trigger a carry operation
        e.EmulateOpcode(0x85);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetRegisters().L == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC L
        e.EmulateOpcode(0x8d);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetRegisters().L == 0x08);
        REQUIRE(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC M")
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

        // MVI M, 0x08
        e.EmulateOpcode(0x36, 0x08);
        REQUIRE(e.GetRegisters().A == 0);
        REQUIRE(e.ReadFromMem(0x2500) == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD M
        // Trigger a carry operation
        e.EmulateOpcode(0x86);
        REQUIRE(e.GetRegisters().A == 0x02);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC M
        e.EmulateOpcode(0x8e);
        CHECK(e.GetRegisters().A == 0x0b);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC A")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xfa);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD A
        // Trigger a carry operation
        e.EmulateOpcode(0x87);
        REQUIRE(e.GetRegisters().A == 0xfa);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC A
        e.EmulateOpcode(0x8f);
        CHECK(e.GetRegisters().A == 0x60);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("PUSH", "[stack]")
{
    Emulator e;
    e.AllocateMemory(0x3000);
    uint16_t sp_before = 0x2500;
    e.SetSP(sp_before);
    REQUIRE(e.GetSP() == sp_before);

    SECTION("Push()")
    {
        e.Push(0x11, 0x22);
        CHECK(e.GetSP() == sp_before - 2);
        CHECK(e.ReadFromMem(sp_before - 1) == 0x11);
        CHECK(e.ReadFromMem(sp_before - 2) == 0x22);
    }
    SECTION("PUSH B")
    {
        // Move values into B, C
        uint8_t value1 = 0x11;
        uint8_t value2 = 0x22;
        e.EmulateOpcode(0x06, value1);
        e.EmulateOpcode(0x0e, value2);
        REQUIRE(e.GetRegisters().B == value1);
        REQUIRE(e.GetRegisters().C == value2);

        uint16_t pc_before = e.GetPC();
        // PUSH B
        e.EmulateOpcode(0xc5);

        CHECK(e.GetSP() == sp_before - 2);
        CHECK(e.ReadFromMem(sp_before - 1) == value1);
        CHECK(e.ReadFromMem(sp_before - 2) == value2);
        CHECK(e.GetPC() == pc_before + 1);
    }
    SECTION("PUSH D")
    {
        // Move values into D, E
        uint8_t value1 = 0x11;
        uint8_t value2 = 0x22;
        e.EmulateOpcode(0x16, value1);
        e.EmulateOpcode(0x1e, value2);
        REQUIRE(e.GetRegisters().D == value1);
        REQUIRE(e.GetRegisters().E == value2);

        uint16_t pc_before = e.GetPC();
        // PUSH D
        e.EmulateOpcode(0xd5);

        CHECK(e.GetSP() == sp_before - 2);
        CHECK(e.ReadFromMem(sp_before - 1) == value1);
        CHECK(e.ReadFromMem(sp_before - 2) == value2);
        CHECK(e.GetPC() == pc_before + 1);
    }
    SECTION("PUSH H")
    {
        // Move values into H, L
        uint8_t value1 = 0x11;
        uint8_t value2 = 0x22;
        e.EmulateOpcode(0x26, value1);
        e.EmulateOpcode(0x2e, value2);
        REQUIRE(e.GetRegisters().H == value1);
        REQUIRE(e.GetRegisters().L == value2);

        uint16_t pc_before = e.GetPC();
        // PUSH H
        e.EmulateOpcode(0xe5);

        CHECK(e.GetSP() == sp_before - 2);
        CHECK(e.ReadFromMem(sp_before - 1) == value1);
        CHECK(e.ReadFromMem(sp_before - 2) == value2);
        CHECK(e.GetPC() == pc_before + 1);
    }
    SECTION("PUSH PSW")
    {
        // Move values into A, B
        uint8_t value1 = 0x11;
        uint8_t value2 = 0xaa;
        e.EmulateOpcode(0x3e, value1);
        e.EmulateOpcode(0x06, value2);

        // Add B to A to set flags (11+aa=bb)
        e.EmulateOpcode(0x80);

        REQUIRE(e.GetRegisters().A == 0xbb);
        REQUIRE(e.GetFlags().z == 0);
        REQUIRE(e.GetFlags().s == 1);
        REQUIRE(e.GetFlags().p == 1);
        REQUIRE(e.GetFlags().cy == 0);

        uint16_t pc_before = e.GetPC();
        // PUSH PSW
        e.EmulateOpcode(0xf5);

        CHECK(e.GetSP() == sp_before - 2);
        CHECK(e.ReadFromMem(sp_before - 1) == 0xbb);
        uint8_t psw = e.ReadFromMem(sp_before - 2);
        CHECK((psw & 0x01) == 0x00); // z bit == 0
        CHECK((psw & 0x02) == 0x02); // s bit == 1
        CHECK((psw & 0x04) == 0x04); // p bit == 1
        CHECK((psw & 0x08) == 0x00); // cy bit == 0
        CHECK(e.GetPC() == pc_before + 1);
    }
}

TEST_CASE("POP", "[stack]")
{
    Emulator e;
    e.AllocateMemory(0x3000);
    uint16_t sp_before = 0x2500;
    e.SetSP(sp_before);
    REQUIRE(e.GetSP() == sp_before);

    SECTION("Pop()")
    {
        e.Push(0x11, 0x22);
        REQUIRE(e.ReadFromMem(sp_before - 1) == 0x11);
        REQUIRE(e.ReadFromMem(sp_before - 2) == 0x22);
        REQUIRE(e.GetSP() == sp_before - 2);

        uint8_t value1 = 0x00;
        uint8_t value2 = 0x00;

        e.Pop(&value1, &value2);

        CHECK(value1 == 0x11);
        CHECK(value2 == 0x22);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("POP B")
    {
        uint8_t value1 = 0x11;
        uint8_t value2 = 0x22;
        e.Push(value1, value2);
        REQUIRE(e.ReadFromMem(sp_before - 1) == value1);
        REQUIRE(e.ReadFromMem(sp_before - 2) == value2);

        uint16_t pc_before = e.GetPC();

        e.EmulateOpcode(0xc1);

        CHECK(e.GetSP() == sp_before);
        CHECK(e.GetRegisters().B == value1);
        CHECK(e.GetRegisters().C == value2);
        CHECK(e.GetPC() == pc_before + 1);
    }
    SECTION("POP D")
    {
        uint8_t value1 = 0x11;
        uint8_t value2 = 0x22;
        e.Push(value1, value2);
        REQUIRE(e.ReadFromMem(sp_before - 1) == value1);
        REQUIRE(e.ReadFromMem(sp_before - 2) == value2);

        uint16_t pc_before = e.GetPC();

        e.EmulateOpcode(0xd1);

        CHECK(e.GetSP() == sp_before);
        CHECK(e.GetRegisters().D == value1);
        CHECK(e.GetRegisters().E == value2);
        CHECK(e.GetPC() == pc_before + 1);
    }
    SECTION("POP H")
    {
        uint8_t value1 = 0x11;
        uint8_t value2 = 0x22;
        e.Push(value1, value2);
        REQUIRE(e.ReadFromMem(sp_before - 1) == value1);
        REQUIRE(e.ReadFromMem(sp_before - 2) == value2);

        uint16_t pc_before = e.GetPC();

        e.EmulateOpcode(0xe1);

        CHECK(e.GetSP() == sp_before);
        CHECK(e.GetRegisters().H == value1);
        CHECK(e.GetRegisters().L == value2);
        CHECK(e.GetPC() == pc_before + 1);
    }
    SECTION("POP PSW")
    {
        // Move values into A, B
        uint8_t value1 = 0x11;
        uint8_t value2 = 0xaa;
        e.EmulateOpcode(0x3e, value1);
        e.EmulateOpcode(0x06, value2);

        // Add B to A to set flags (11+aa=bb)
        // z=0, s=1, p=1, cy=0 (psw = 00000110 = 0x06)
        e.EmulateOpcode(0x80);

        // PUSH PSW
        e.EmulateOpcode(0xf5);
        // SUB A to clear register and reset flags
        e.EmulateOpcode(0x97, 0x00);

        REQUIRE(e.GetSP() == sp_before - 2);
        REQUIRE(e.ReadFromMem(sp_before - 1) == 0xbb);
        REQUIRE(e.ReadFromMem(sp_before - 2) == 0x06);
        REQUIRE(e.GetFlags().z == 1);
        REQUIRE(e.GetFlags().s == 0);
        REQUIRE(e.GetFlags().p == 1);
        REQUIRE(e.GetFlags().cy == 0);

        uint16_t pc_before = e.GetPC();
        // POP PSW
        e.EmulateOpcode(0xf1);
        uint16_t pc_after = e.GetPC();

        CHECK(e.GetSP() == sp_before);
        CHECK(e.GetRegisters().A == 0xbb);
        CHECK(e.GetFlags().z == 0);
        CHECK(e.GetFlags().s == 1);
        CHECK(e.GetFlags().p == 1);
        CHECK(e.GetFlags().cy == 0);
        CHECK(pc_after == pc_before + 1);
    }
}

TEST_CASE("Call", "[stack][subroutine][call][opcode]")
{
    Emulator e;
    e.AllocateMemory(0x3000);

    // Increase PC to 0x0102
    for (int i = 0; i < 0x0102; i++)
        e.EmulateOpcode(0x00);

    uint16_t pc_before = e.GetPC();
    REQUIRE(pc_before == 0x0102);

    uint8_t ret_high = (pc_before + 3) >> 8;
    uint8_t ret_low = (pc_before + 3) & 0x00ff;

    uint16_t sp_before = 0x2500;
    e.SetSP(sp_before);

    SECTION("Call()")
    {
        e.Call(0x12, 0x34);

        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CALL")
    {
        e.EmulateOpcode(0xcd, 0x34, 0x12);

        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CC (cy=0)")
    {
        // set cy = 0
        REQUIRE(e.GetFlags().cy == 0);

        e.EmulateOpcode(0xdc, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("CC (cy=1)")
    {
        // set cy = 1
        e.EmulateOpcode(0x37);
        pc_before = e.GetPC();
        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;

        REQUIRE(e.GetFlags().cy == 1);

        e.EmulateOpcode(0xdc, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CNC (cy=0)")
    {
        REQUIRE(e.GetFlags().cy == 0);

        e.EmulateOpcode(0xd4, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CNC (cy=1)")
    {
        // set cy = 1
        e.EmulateOpcode(0x37);
        pc_before = e.GetPC();
        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;
        REQUIRE(e.GetFlags().cy == 1);

        e.EmulateOpcode(0xd4, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
}

TEST_CASE("Return", "[stack][subroutine][return][opcode]")
{
    Emulator e;
    e.AllocateMemory(0x3000);

    // Increase PC to 0x0102 with NOP instructions
    for (int i = 0; i < 0x0102; i++)
        e.EmulateOpcode(0x00);

    uint16_t ret_addr = e.GetPC() + 3;
    // return address 0x0105
    uint8_t ret_high = (ret_addr) >> 8;
    uint8_t ret_low = (ret_addr) & 0x00ff;

    uint16_t sp_start = 0x2500;
    e.SetSP(sp_start);

    e.Call(0x12, 0x34);

    // increment PC with NOP instructions
    for (int i = 0; i < 25; i++)
        e.EmulateOpcode(0x00);

    uint16_t pc_before = e.GetPC();
    uint16_t sp_before = e.GetSP();
    REQUIRE(pc_before == 0x124d);
    REQUIRE(sp_before == sp_start - 2);

    SECTION("Return()")
    {
        e.Return();

        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RET")
    {
        e.EmulateOpcode(0xc9);

        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RC (cy=0)")
    {
        e.EmulateOpcode(0xd8);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RC (cy=1)")
    {
        // set cy = 1
        e.EmulateOpcode(0x37);
        pc_before = e.GetPC();

        e.EmulateOpcode(0xd8);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RNC (cy=0)")
    {
        e.EmulateOpcode(0xd0);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RNC (cy=1)")
    {
        // set cy = 1
        e.EmulateOpcode(0x37);
        pc_before = e.GetPC();

        e.EmulateOpcode(0xd0);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
}
