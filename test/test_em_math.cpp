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

TEST_CASE("Add immediate", "[opcode][adi][math]")
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

    SECTION("ADI D8 - Set carry")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x30);
        REQUIRE(e.GetRegisters().A == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADI 0x30
        e.EmulateOpcode(0xc6, 0xff);
        CHECK(e.GetRegisters().A == 0x2f);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetPC() == test_pc + 2);
    }
}

TEST_CASE("Add immediate with carry", "[opcode][aci][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("ACI D8 - No carry")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x30);
        REQUIRE(e.GetRegisters().A == 0x30);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ACI 0x30
        e.EmulateOpcode(0xce, 0x30);
        CHECK(e.GetRegisters().A == 0x60);
        CHECK(e.GetPC() == test_pc + 2);
    }

    SECTION("ACI D8")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xca);
        REQUIRE(e.GetRegisters().A == 0xca);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD A
        e.EmulateOpcode(0x87);
        REQUIRE(e.GetRegisters().A == 0x94);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ACI 0x30
        e.EmulateOpcode(0xce, 0x30);
        CHECK(e.GetRegisters().A == 0xc5);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 2);
    }

    SECTION("ACI D8 - Set carry")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD A
        e.EmulateOpcode(0x87);
        REQUIRE(e.GetRegisters().A == 0xfe);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ACI 0x30
        e.EmulateOpcode(0xce, 0x30);
        CHECK(e.GetRegisters().A == 0x2f);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetPC() == test_pc + 2);
    }
}

TEST_CASE("Subtract immediate from accumulator", "[opcode][sui][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("SUI D8")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x60);
        REQUIRE(e.GetRegisters().A == 0x60);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUI 0x30
        e.EmulateOpcode(0xd6, 0x30);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetPC() == test_pc + 2);
    }

    SECTION("SUI D8 - Set carry")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x2f);
        REQUIRE(e.GetRegisters().A == 0x2f);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUI 0x30
        e.EmulateOpcode(0xd6, 0x30);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetPC() == test_pc + 2);
    }
}

TEST_CASE("Subtract immediate from accumulator with borrow", "[opcode][sbi][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("SBI D8 - No borrow")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x60);
        REQUIRE(e.GetRegisters().A == 0x60);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUI 0x30
        e.EmulateOpcode(0xde, 0x30);
        CHECK(e.GetRegisters().A == 0x30);
        CHECK(e.GetPC() == test_pc + 2);
    }

    SECTION("SBI D8")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xca);
        REQUIRE(e.GetRegisters().A == 0xca);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD A
        e.EmulateOpcode(0x87);
        REQUIRE(e.GetRegisters().A == 0x94);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // SBI 0x30
        e.EmulateOpcode(0xde, 0x30);
        CHECK(e.GetRegisters().A == 0x63);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 2);
    }

    SECTION("SBI D8 - Set carry")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD A
        e.EmulateOpcode(0x87);
        REQUIRE(e.GetRegisters().A == 0xfe);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // SBI 0xfe
        e.EmulateOpcode(0xde, 0xfe);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetFlags().cy == 1);
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
        CHECK(e.GetFlags().cy == 0);
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
        CHECK(e.GetFlags().cy == 0);
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
        CHECK(e.GetFlags().cy == 0);
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
        CHECK(e.GetFlags().cy == 0);
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
        CHECK(e.GetFlags().cy == 0);
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
        CHECK(e.GetFlags().cy == 0);
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
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("ADC A - Double carry")
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
        REQUIRE(e.GetRegisters().A == 0xf4);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // ADC A
        e.EmulateOpcode(0x8f);
        CHECK(e.GetRegisters().A == 0xe9);
        CHECK(e.GetFlags().cy == 1); // 0xf4 + 0xf4 + 0x01 = 0x01e9 (still overflows)
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("Subtraction", "[opcode][sub][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("SUB B")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI B
        e.EmulateOpcode(0x06, 0x12);
        REQUIRE(e.GetRegisters().B == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB B
        e.EmulateOpcode(0x90);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetRegisters().B == 0x12);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB C")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI C
        e.EmulateOpcode(0x0e, 0x12);
        REQUIRE(e.GetRegisters().C == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB C
        e.EmulateOpcode(0x91);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetRegisters().C == 0x12);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB D")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI D
        e.EmulateOpcode(0x16, 0x12);
        REQUIRE(e.GetRegisters().D == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB D
        e.EmulateOpcode(0x92);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetRegisters().D == 0x12);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB E")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI E
        e.EmulateOpcode(0x1e, 0x12);
        REQUIRE(e.GetRegisters().E == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB E
        e.EmulateOpcode(0x93);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetRegisters().E == 0x12);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB H")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI H
        e.EmulateOpcode(0x26, 0x12);
        REQUIRE(e.GetRegisters().H == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB H
        e.EmulateOpcode(0x94);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetRegisters().H == 0x12);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB L")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x12);
        REQUIRE(e.GetRegisters().L == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB L
        e.EmulateOpcode(0x95);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetRegisters().L == 0x12);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB M")
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

        // MVI A, 0x35
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI M, 0x12
        e.EmulateOpcode(0x36, 0x12);
        REQUIRE(e.ReadFromMem(0x2500) == 0x12);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB M
        e.EmulateOpcode(0x96);
        CHECK(e.GetRegisters().A == 0x23);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SUB A")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x35);
        REQUIRE(e.GetRegisters().A == 0x35);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // SUB A
        e.EmulateOpcode(0x97);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("Subtract with borrow", "[opcode][sbb][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("SBB B")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().B == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI B
        e.EmulateOpcode(0x06, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD B
        // Trigger a carry operation
        e.EmulateOpcode(0x80);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB B
        e.EmulateOpcode(0x98);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetRegisters().B == 0x08);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB B - Overflow")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().B == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI B
        e.EmulateOpcode(0x06, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD B
        // Trigger a carry operation
        e.EmulateOpcode(0x80);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().B == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // SBB B
        e.EmulateOpcode(0x98);
        CHECK(e.GetRegisters().A == 0xfe);
        CHECK(e.GetRegisters().B == 0x08);
        CHECK(e.GetFlags().cy == 1); // Caused overflow
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB C")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().C == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI C
        e.EmulateOpcode(0x0e, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().C == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD C
        // Trigger a carry operation
        e.EmulateOpcode(0x81);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().C == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.GetRegisters().C == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB C
        e.EmulateOpcode(0x99);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetRegisters().C == 0x08);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB D")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().D == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI D
        e.EmulateOpcode(0x16, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().D == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD D
        // Trigger a carry operation
        e.EmulateOpcode(0x82);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().D == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.GetRegisters().D == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB D
        e.EmulateOpcode(0x9a);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetRegisters().D == 0x08);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB E")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().E == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI E
        e.EmulateOpcode(0x1e, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().E == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD E
        // Trigger a carry operation
        e.EmulateOpcode(0x83);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().E == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.GetRegisters().E == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB E
        e.EmulateOpcode(0x9b);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetRegisters().E == 0x08);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB H")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().H == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI H
        e.EmulateOpcode(0x26, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().H == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD H
        // Trigger a carry operation
        e.EmulateOpcode(0x84);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().H == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.GetRegisters().H == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB H
        e.EmulateOpcode(0x9c);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetRegisters().H == 0x08);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB L")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().L == 0);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x08);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetRegisters().L == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD L
        // Trigger a carry operation
        e.EmulateOpcode(0x85);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.GetRegisters().L == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.GetRegisters().L == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB L
        e.EmulateOpcode(0x9d);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetRegisters().L == 0x08);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("SBB M")
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

        // MVI A, 0xff
        e.EmulateOpcode(0x3e, 0xff);
        REQUIRE(e.GetRegisters().A == 0xff);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // MVI M, 0x08
        e.EmulateOpcode(0x36, 0x08);
        REQUIRE(e.ReadFromMem(0x2500) == 0x08);
        REQUIRE(e.GetPC() == test_pc + 2);
        test_pc = e.GetPC();

        // ADD M
        // Trigger a carry operation
        e.EmulateOpcode(0x86);
        REQUIRE(e.GetRegisters().A == 0x07);
        REQUIRE(e.ReadFromMem(0x2500) == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == test_pc + 1);
        test_pc = e.GetPC();

        // MVI A - To prevent overflow
        e.EmulateOpcode(0x3e, 0x0a);
        REQUIRE(e.GetRegisters().A == 0x0a);
        REQUIRE(e.ReadFromMem(0x2500) == 0x08);
        REQUIRE(e.GetFlags().cy == 1);
        test_pc = e.GetPC();

        // SBB M
        e.EmulateOpcode(0x9e);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

// NOTE: INR only affects auxiliary carry flag (not regular carry), which isn't used in Space Invaders
TEST_CASE("Increment register", "[opcode][inr][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("INR A")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().A == 0x00);
        e.EmulateOpcode(0x3c);
        CHECK(e.GetRegisters().A == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR B")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().B == 0x00);
        e.EmulateOpcode(0x04);
        CHECK(e.GetRegisters().B == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR C")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().C == 0x00);
        e.EmulateOpcode(0x0c);
        CHECK(e.GetRegisters().C == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR D")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().D == 0x00);
        e.EmulateOpcode(0x14);
        CHECK(e.GetRegisters().D == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR E")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().E == 0x00);
        e.EmulateOpcode(0x1c);
        CHECK(e.GetRegisters().E == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR H")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().H == 0x00);
        e.EmulateOpcode(0x24);
        CHECK(e.GetRegisters().H == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR L")
    {
        int test_pc = pc;

        REQUIRE(e.GetRegisters().L == 0x00);
        e.EmulateOpcode(0x2c);
        CHECK(e.GetRegisters().L == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INR M")
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

        REQUIRE(e.ReadFromHL() == 0x00);
        e.EmulateOpcode(0x34);
        CHECK(e.ReadFromHL() == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("INX", "[opcode][inx][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("INX B")
    {
        int test_pc = pc;

        // MVI B, 0xff
        // MVI C, 0xff
        e.EmulateOpcode(0x06, 0xff);
        e.EmulateOpcode(0x0e, 0xff);
        REQUIRE(e.GetRegisters().B == 0xff);
        REQUIRE(e.GetRegisters().C == 0xff);
        test_pc = e.GetPC();

        // INX B
        e.EmulateOpcode(0x03);
        CHECK(e.GetRegisters().B == 0x00);
        CHECK(e.GetRegisters().C == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INX B - No overflow")
    {
        int test_pc = pc;

        // MVI B, 0x00
        // MVI C, 0xff
        e.EmulateOpcode(0x06, 0x00);
        e.EmulateOpcode(0x0e, 0xff);
        REQUIRE(e.GetRegisters().B == 0x00);
        REQUIRE(e.GetRegisters().C == 0xff);
        test_pc = e.GetPC();

        // INX B
        e.EmulateOpcode(0x03);
        CHECK(e.GetRegisters().B == 0x01);
        CHECK(e.GetRegisters().C == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INX D")
    {
        int test_pc = pc;

        // MVI D, 0xff
        // MVI E, 0xff
        e.EmulateOpcode(0x16, 0xff);
        e.EmulateOpcode(0x1e, 0xff);
        REQUIRE(e.GetRegisters().D == 0xff);
        REQUIRE(e.GetRegisters().E == 0xff);
        test_pc = e.GetPC();

        // INX D
        e.EmulateOpcode(0x13);
        CHECK(e.GetRegisters().D == 0x00);
        CHECK(e.GetRegisters().E == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INX D - No overflow")
    {
        int test_pc = pc;

        // MVI D, 0x00
        // MVI E, 0xff
        e.EmulateOpcode(0x16, 0x00);
        e.EmulateOpcode(0x1e, 0xff);
        REQUIRE(e.GetRegisters().D == 0x00);
        REQUIRE(e.GetRegisters().E == 0xff);
        test_pc = e.GetPC();

        // INX D
        e.EmulateOpcode(0x13);
        CHECK(e.GetRegisters().D == 0x01);
        CHECK(e.GetRegisters().E == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INX H")
    {
        int test_pc = pc;

        // MVI H, 0xff
        // MVI L, 0xff
        e.EmulateOpcode(0x26, 0xff);
        e.EmulateOpcode(0x2e, 0xff);
        REQUIRE(e.GetRegisters().H == 0xff);
        REQUIRE(e.GetRegisters().L == 0xff);
        test_pc = e.GetPC();

        // INX H
        e.EmulateOpcode(0x23);
        CHECK(e.GetRegisters().H == 0x00);
        CHECK(e.GetRegisters().L == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INX H - No overflow")
    {
        int test_pc = pc;

        // MVI H, 0x00
        // MVI L, 0xff
        e.EmulateOpcode(0x26, 0x00);
        e.EmulateOpcode(0x2e, 0xff);
        REQUIRE(e.GetRegisters().H == 0x00);
        REQUIRE(e.GetRegisters().L == 0xff);
        test_pc = e.GetPC();

        // INX H
        e.EmulateOpcode(0x23);
        CHECK(e.GetRegisters().H == 0x01);
        CHECK(e.GetRegisters().L == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("INX SP")
    {
        int test_pc = pc;

        REQUIRE(e.GetSP() == 0x00);

        // INX SP
        e.EmulateOpcode(0x33);
        CHECK(e.GetSP() == 0x01);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("Decrement register", "[opcode][dcr][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("DCR A")
    {
        int test_pc = pc;

        // INR A
        e.EmulateOpcode(0x3c);
        REQUIRE(e.GetRegisters().A == 0x01);
        test_pc = e.GetPC();

        // DCR A
        e.EmulateOpcode(0x3d);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR A - Already zero")
    {
        int test_pc = pc;

        // DCR A
        e.EmulateOpcode(0x3d);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR B")
    {
        int test_pc = pc;

        // INR B
        e.EmulateOpcode(0x04);
        REQUIRE(e.GetRegisters().B == 0x01);
        test_pc = e.GetPC();

        // DCR B
        e.EmulateOpcode(0x05);
        CHECK(e.GetRegisters().B == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR B - Already zero")
    {
        int test_pc = pc;

        // DCR B
        e.EmulateOpcode(0x05);
        CHECK(e.GetRegisters().B == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR C")
    {
        int test_pc = pc;

        // INR C
        e.EmulateOpcode(0x0c);
        REQUIRE(e.GetRegisters().C == 0x01);
        test_pc = e.GetPC();

        // DCR C
        e.EmulateOpcode(0x0d);
        CHECK(e.GetRegisters().C == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR C - Already zero")
    {
        int test_pc = pc;

        // DCR C
        e.EmulateOpcode(0x0d);
        CHECK(e.GetRegisters().C == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR D")
    {
        int test_pc = pc;

        // INR D
        e.EmulateOpcode(0x14);
        REQUIRE(e.GetRegisters().D == 0x01);
        test_pc = e.GetPC();

        // DCR D
        e.EmulateOpcode(0x15);
        CHECK(e.GetRegisters().D == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR D - Already zero")
    {
        int test_pc = pc;

        // DCR D
        e.EmulateOpcode(0x15);
        CHECK(e.GetRegisters().D == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR E")
    {
        int test_pc = pc;

        // INR E
        e.EmulateOpcode(0x1c);
        REQUIRE(e.GetRegisters().E == 0x01);
        test_pc = e.GetPC();

        // DCR E
        e.EmulateOpcode(0x1d);
        CHECK(e.GetRegisters().E == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR E - Already zero")
    {
        int test_pc = pc;

        // DCR E
        e.EmulateOpcode(0x1d);
        CHECK(e.GetRegisters().E == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR H")
    {
        int test_pc = pc;

        // INR H
        e.EmulateOpcode(0x24);
        REQUIRE(e.GetRegisters().H == 0x01);
        test_pc = e.GetPC();

        // DCR H
        e.EmulateOpcode(0x25);
        CHECK(e.GetRegisters().H == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR H - Already zero")
    {
        int test_pc = pc;

        // DCR H
        e.EmulateOpcode(0x25);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR L")
    {
        int test_pc = pc;

        // INR L
        e.EmulateOpcode(0x2c);
        REQUIRE(e.GetRegisters().L == 0x01);
        test_pc = e.GetPC();

        // DCR L
        e.EmulateOpcode(0x2d);
        CHECK(e.GetRegisters().L == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR L - Already zero")
    {
        int test_pc = pc;

        // DCR L
        e.EmulateOpcode(0x2d);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR M")
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

        // INR M
        e.EmulateOpcode(0x34);
        REQUIRE(e.ReadFromHL() == 0x01);
        test_pc = e.GetPC();

        // DCR M
        e.EmulateOpcode(0x35);
        REQUIRE(e.ReadFromHL() == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCR M - Already zero")
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

        // DCR M
        e.EmulateOpcode(0x35);
        REQUIRE(e.ReadFromHL() == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("DCX", "[opcode][dcx][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("DCX B")
    {
        int test_pc = pc;

        // MVI B, 0x01
        // MVI C, 0x00
        e.EmulateOpcode(0x06, 0x01);
        e.EmulateOpcode(0x0e, 0x00);
        REQUIRE(e.GetRegisters().B == 0x01);
        REQUIRE(e.GetRegisters().C == 0x00);
        test_pc = e.GetPC();

        // DCX B
        e.EmulateOpcode(0x0b);
        CHECK(e.GetRegisters().B == 0x00);
        CHECK(e.GetRegisters().C == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCX B - Underflow")
    {
        int test_pc = pc;

        // MVI B, 0x00
        // MVI C, 0x00
        e.EmulateOpcode(0x06, 0x00);
        e.EmulateOpcode(0x0e, 0x00);
        REQUIRE(e.GetRegisters().B == 0x00);
        REQUIRE(e.GetRegisters().C == 0x00);
        test_pc = e.GetPC();

        // DCX B
        e.EmulateOpcode(0x0b);
        CHECK(e.GetRegisters().B == 0xff);
        CHECK(e.GetRegisters().C == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCX D")
    {
        int test_pc = pc;

        // MVI D, 0x01
        // MVI E, 0x00
        e.EmulateOpcode(0x16, 0x01);
        e.EmulateOpcode(0x1e, 0x00);
        REQUIRE(e.GetRegisters().D == 0x01);
        REQUIRE(e.GetRegisters().E == 0x00);
        test_pc = e.GetPC();

        // DCX D
        e.EmulateOpcode(0x1b);
        CHECK(e.GetRegisters().D == 0x00);
        CHECK(e.GetRegisters().E == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCX D - Underflow")
    {
        int test_pc = pc;

        // MVI D, 0x00
        // MVI E, 0x00
        e.EmulateOpcode(0x16, 0x00);
        e.EmulateOpcode(0x1e, 0x00);
        REQUIRE(e.GetRegisters().D == 0x00);
        REQUIRE(e.GetRegisters().E == 0x00);
        test_pc = e.GetPC();

        // DCX D
        e.EmulateOpcode(0x1b);
        CHECK(e.GetRegisters().D == 0xff);
        CHECK(e.GetRegisters().E == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCX H")
    {
        int test_pc = pc;

        // MVI H, 0x01
        // MVI L, 0x00
        e.EmulateOpcode(0x26, 0x01);
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().H == 0x01);
        REQUIRE(e.GetRegisters().L == 0x00);
        test_pc = e.GetPC();

        // DCX H
        e.EmulateOpcode(0x2b);
        CHECK(e.GetRegisters().H == 0x00);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCX H - Underflow")
    {
        int test_pc = pc;

        // MVI H, 0x00
        // MVI L, 0x00
        e.EmulateOpcode(0x26, 0x00);
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().H == 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        test_pc = e.GetPC();

        // DCX H
        e.EmulateOpcode(0x2b);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DCX SP")
    {
        int test_pc = pc;

        REQUIRE(e.GetSP() == 0x00);

        // INX SP
        e.EmulateOpcode(0x33);
        REQUIRE(e.GetSP() == 0x01);
        test_pc = e.GetPC();

        // DCX SP
        e.EmulateOpcode(0x3b);
        REQUIRE(e.GetSP() == 0x00);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("Double Addition", "[opcode][dad][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("DAD B - Empty HL")
    {
        int test_pc = pc;

        // MVI B
        // MVI C
        e.EmulateOpcode(0x06, 0x30);
        e.EmulateOpcode(0x0e, 0x30);
        REQUIRE(e.GetRegisters().B == 0x30);
        REQUIRE(e.GetRegisters().C == 0x30);
        REQUIRE(e.GetRegisters().H == 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        test_pc = e.GetPC();

        // DAD B
        e.EmulateOpcode(0x09);
        CHECK(e.GetRegisters().B == 0x30);
        CHECK(e.GetRegisters().C == 0x30);
        CHECK(e.GetRegisters().H == 0x30);
        CHECK(e.GetRegisters().L == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAD B")
    {
        int test_pc = pc;

        // MVI B
        // MVI C
        // MVI H
        // MVI L
        e.EmulateOpcode(0x06, 0x30);
        e.EmulateOpcode(0x0e, 0x30);
        e.EmulateOpcode(0x26, 0x30);
        e.EmulateOpcode(0x2e, 0x30);
        REQUIRE(e.GetRegisters().B == 0x30);
        REQUIRE(e.GetRegisters().C == 0x30);
        REQUIRE(e.GetRegisters().H == 0x30);
        REQUIRE(e.GetRegisters().L == 0x30);
        test_pc = e.GetPC();

        // DAD B
        e.EmulateOpcode(0x09);
        CHECK(e.GetRegisters().B == 0x30);
        CHECK(e.GetRegisters().C == 0x30);
        CHECK(e.GetRegisters().H == 0x60);
        CHECK(e.GetRegisters().L == 0x60);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAD B - Trigger carry")
    {
        int test_pc = pc;

        // MVI B
        // MVI C
        // MVI H
        // MVI L
        e.EmulateOpcode(0x06, 0x30);
        e.EmulateOpcode(0x0e, 0x30);
        e.EmulateOpcode(0x26, 0xff);
        e.EmulateOpcode(0x2e, 0xff);
        REQUIRE(e.GetRegisters().B == 0x30);
        REQUIRE(e.GetRegisters().C == 0x30);
        REQUIRE(e.GetRegisters().H == 0xff);
        REQUIRE(e.GetRegisters().L == 0xff);
        test_pc = e.GetPC();

        // DAD B
        e.EmulateOpcode(0x09);
        CHECK(e.GetRegisters().B == 0x30);
        CHECK(e.GetRegisters().C == 0x30);
        CHECK(e.GetRegisters().H == 0x30);
        CHECK(e.GetRegisters().L == 0x2f);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAD D - Empty HL")
    {
        int test_pc = pc;

        // MVI D
        // MVI E
        e.EmulateOpcode(0x16, 0x30);
        e.EmulateOpcode(0x1e, 0x30);
        REQUIRE(e.GetRegisters().D == 0x30);
        REQUIRE(e.GetRegisters().E == 0x30);
        REQUIRE(e.GetRegisters().H == 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        test_pc = e.GetPC();

        // DAD D
        e.EmulateOpcode(0x19);
        CHECK(e.GetRegisters().D == 0x30);
        CHECK(e.GetRegisters().E == 0x30);
        CHECK(e.GetRegisters().H == 0x30);
        CHECK(e.GetRegisters().L == 0x30);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAD D")
    {
        int test_pc = pc;

        // MVI D
        // MVI E
        // MVI H
        // MVI L
        e.EmulateOpcode(0x16, 0x30);
        e.EmulateOpcode(0x1e, 0x30);
        e.EmulateOpcode(0x26, 0x30);
        e.EmulateOpcode(0x2e, 0x30);
        REQUIRE(e.GetRegisters().D == 0x30);
        REQUIRE(e.GetRegisters().E == 0x30);
        REQUIRE(e.GetRegisters().H == 0x30);
        REQUIRE(e.GetRegisters().L == 0x30);
        test_pc = e.GetPC();

        // DAD D
        e.EmulateOpcode(0x19);
        CHECK(e.GetRegisters().D == 0x30);
        CHECK(e.GetRegisters().E == 0x30);
        CHECK(e.GetRegisters().H == 0x60);
        CHECK(e.GetRegisters().L == 0x60);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAD H")
    {
        int test_pc = pc;

        // MVI H
        // MVI L
        e.EmulateOpcode(0x26, 0x30);
        e.EmulateOpcode(0x2e, 0x30);
        REQUIRE(e.GetRegisters().H == 0x30);
        REQUIRE(e.GetRegisters().L == 0x30);
        test_pc = e.GetPC();

        // DAD H
        e.EmulateOpcode(0x29);
        CHECK(e.GetRegisters().H == 0x60);
        CHECK(e.GetRegisters().L == 0x60);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAD SP")
    {
        int test_pc = pc;

        // MVI H
        // MVI L
        // INX SP
        e.EmulateOpcode(0x26, 0x30);
        e.EmulateOpcode(0x2e, 0x30);
        e.EmulateOpcode(0x33);
        REQUIRE(e.GetRegisters().H == 0x30);
        REQUIRE(e.GetRegisters().L == 0x30);
        REQUIRE(e.GetSP() == 1);
        test_pc = e.GetPC();

        // DAD SP
        e.EmulateOpcode(0x39);
        CHECK(e.GetRegisters().H == 0x30);
        CHECK(e.GetRegisters().L == 0x31);
        CHECK(e.GetPC() == test_pc + 1);
    }
}

TEST_CASE("Decimal Adjust Accumulator", "[opcode][daa][math]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("DAA - No adjustment")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x32);
        REQUIRE(e.GetRegisters().A == 0x32);
        test_pc = e.GetPC();

        // DAA
        e.EmulateOpcode(0x27);
        CHECK(e.GetRegisters().A == 0x32);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAA - Lower nibble adjustment")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0x3a);
        REQUIRE(e.GetRegisters().A == 0x3a);
        test_pc = e.GetPC();

        // DAA
        e.EmulateOpcode(0x27);
        CHECK(e.GetRegisters().A == 0x40);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAA - Upper nibble adjustment")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xa0);
        REQUIRE(e.GetRegisters().A == 0xa0);
        test_pc = e.GetPC();

        // DAA
        e.EmulateOpcode(0x27);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetPC() == test_pc + 1);
    }

    SECTION("DAA - Both nibbles adjusted")
    {
        int test_pc = pc;

        // MVI A
        e.EmulateOpcode(0x3e, 0xaa);
        REQUIRE(e.GetRegisters().A == 0xaa);
        test_pc = e.GetPC();

        // DAA
        e.EmulateOpcode(0x27);
        CHECK(e.GetRegisters().A == 0x10);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetPC() == test_pc + 1);
    }
}
