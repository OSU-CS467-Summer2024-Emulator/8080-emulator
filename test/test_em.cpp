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

TEST_CASE("0x00 NOP", "[opcode][nop]")
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

// Other opcodes that don't have instructions
TEST_CASE("Invalid opcodes", "[opcode][nop][invalid][.]")
{
    Emulator e;
    Registers registers_before = e.GetRegisters();
    Flags flags_before = e.GetFlags();
    int pc_before = e.GetPC();
    int sp_before = e.GetSP();

    SECTION("0x08")
    {
        e.EmulateOpcode(0x08);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0x10")
    {
        e.EmulateOpcode(0x10);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0x18")
    {
        e.EmulateOpcode(0x18);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0x20")
    {
        e.EmulateOpcode(0x20);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0x28")
    {
        e.EmulateOpcode(0x28);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0x30")
    {
        e.EmulateOpcode(0x30);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0x38")
    {
        e.EmulateOpcode(0x38);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0xcb")
    {
        e.EmulateOpcode(0xcb);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0xdd")
    {
        e.EmulateOpcode(0xdd);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0xed")
    {
        e.EmulateOpcode(0xed);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
    SECTION("0xfd")
    {
        e.EmulateOpcode(0xfd);

        REQUIRE(e.GetRegisters() == registers_before);
        REQUIRE(e.GetFlags() == flags_before);
        REQUIRE(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetSP() == sp_before);
    }
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
    SECTION("CZ (z=0)")
    {
        REQUIRE(e.GetFlags().z == 0);

        e.EmulateOpcode(0xcc, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("CZ (z=1)")
    {
        // set z = 1
        // A = 0x22 - 0x22 = 0x00
        e.EmulateOpcode(0x3e, 0x22);
        e.EmulateOpcode(0x06, 0x22);
        e.EmulateOpcode(0x90);

        pc_before = e.GetPC();
        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;

        REQUIRE(e.GetFlags().z == 1);

        e.EmulateOpcode(0xcc, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CNZ (z=0)")
    {
        REQUIRE(e.GetFlags().z == 0);

        e.EmulateOpcode(0xc4, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CNZ (z=1)")
    {
        // set z = 1
        // A = 0x22 - 0x22 = 0x00
        e.EmulateOpcode(0x3e, 0x22);
        e.EmulateOpcode(0x06, 0x22);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;
        REQUIRE(e.GetFlags().z == 1);

        e.EmulateOpcode(0xc4, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("CM (s=0)")
    {
        REQUIRE(e.GetFlags().s == 0);

        e.EmulateOpcode(0xfc, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("CM (s=1)")
    {
        // set s = 1
        // A = 0xff - 0x11 = 0xee
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x06, 0x11);
        e.EmulateOpcode(0x90);

        pc_before = e.GetPC();
        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;

        REQUIRE(e.GetFlags().s == 1);

        e.EmulateOpcode(0xfc, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CP (s=0)")
    {
        REQUIRE(e.GetFlags().s == 0);

        e.EmulateOpcode(0xf4, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CP (s=1)")
    {
        // set s = 1
        // A = 0xff - 0x11 = 0xee
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x06, 0x11);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;

        REQUIRE(e.GetFlags().s == 1);

        e.EmulateOpcode(0xf4, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("CPE (p=0)")
    {
        REQUIRE(e.GetFlags().p == 0);

        e.EmulateOpcode(0xec, 0x34, 0x12);

        // No call
        CHECK(e.GetPC() == pc_before + 3);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("CPE (p=1)")
    {
        // set p = 1: A-- -> 0xff (even parity)
        e.EmulateOpcode(0x3d);

        pc_before = e.GetPC();
        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;

        REQUIRE(e.GetFlags().p == 1);

        e.EmulateOpcode(0xec, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CPO (p=0)")
    {
        REQUIRE(e.GetFlags().p == 0);

        e.EmulateOpcode(0xe4, 0x34, 0x12);

        // Call
        CHECK(e.GetPC() == 0x1234);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("CPO (p=1)")
    {
        // set p = 1: A-- -> 0xff (even parity)
        e.EmulateOpcode(0x3d);

        pc_before = e.GetPC();

        ret_high = (pc_before + 3) >> 8;
        ret_low = (pc_before + 3) & 0x00ff;
        REQUIRE(e.GetFlags().p == 1);

        e.EmulateOpcode(0xe4, 0x34, 0x12);

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
        REQUIRE(e.GetFlags().cy == 0);

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

        REQUIRE(e.GetFlags().cy == 1);

        e.EmulateOpcode(0xd8);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RNC (cy=0)")
    {
        REQUIRE(e.GetFlags().cy == 0);

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

        REQUIRE(e.GetFlags().cy == 1);

        e.EmulateOpcode(0xd0);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RZ (z=0)")
    {
        REQUIRE(e.GetFlags().z == 0);

        e.EmulateOpcode(0xc8);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RZ (z=1)")
    {
        // set z = 1
        // A = 0x22 - 0x22 = 0x00
        e.EmulateOpcode(0x3e, 0x22);
        e.EmulateOpcode(0x06, 0x22);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().z == 1);

        e.EmulateOpcode(0xc8);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RNZ (z=0)")
    {
        REQUIRE(e.GetFlags().z == 0);

        e.EmulateOpcode(0xc0);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RNZ (z=1)")
    {
        // set z = 1
        // A = 0x22 - 0x22 = 0x00
        e.EmulateOpcode(0x3e, 0x22);
        e.EmulateOpcode(0x06, 0x22);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().z == 1);

        e.EmulateOpcode(0xc0);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RM (s=0)")
    {
        REQUIRE(e.GetFlags().s == 0);

        e.EmulateOpcode(0xf8);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RM (s=1)")
    {
        // set s = 1
        // A = 0xff - 0x11 = 0xee
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x06, 0x11);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().s == 1);

        e.EmulateOpcode(0xf8);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RP (s=0)")
    {
        REQUIRE(e.GetFlags().s == 0);

        e.EmulateOpcode(0xf0);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RP (s=1)")
    {
        // set s = 1
        // A = 0xff - 0x11 = 0xee
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x06, 0x11);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().s == 1);

        e.EmulateOpcode(0xf0);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RPE (p=0)")
    {
        REQUIRE(e.GetFlags().p == 0);

        e.EmulateOpcode(0xe8);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
    SECTION("RPE (p=1)")
    {
        // set p = 1: A-- -> 0xff (even parity)
        e.EmulateOpcode(0x3d);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().p == 1);

        e.EmulateOpcode(0xe8);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RPO (p=0)")
    {
        REQUIRE(e.GetFlags().p == 0);

        e.EmulateOpcode(0xe0);

        // return
        CHECK(e.GetPC() == ret_addr);
        CHECK(e.GetSP() == sp_start);
    }
    SECTION("RPO (p=1)")
    {
        // set p = 1: A-- -> 0xff (even parity)
        e.EmulateOpcode(0x3d);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().p == 1);

        e.EmulateOpcode(0xe0);

        // don't return
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetSP() == sp_before);
    }
}

TEST_CASE("Jump", "[jump][opcode]")
{
    Emulator e;

    e.AllocateMemory(0x3000);

    // Increase PC to 0x0102
    for (int i = 0; i < 0x0102; i++)
        e.EmulateOpcode(0x00);

    uint16_t pc_before = e.GetPC();
    REQUIRE(pc_before == 0x0102);

    uint16_t sp_before = 0x2500;
    e.SetSP(sp_before);

    SECTION("JMP")
    {
        e.EmulateOpcode(0xc3, 0x34, 0x12);

        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JC (cy=0)")
    {
        // set cy = 0
        REQUIRE(e.GetFlags().cy == 0);

        e.EmulateOpcode(0xda, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JC (cy=1)")
    {
        // set cy = 1
        e.EmulateOpcode(0x37);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().cy == 1);

        e.EmulateOpcode(0xda, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JNC (cy=0)")
    {
        REQUIRE(e.GetFlags().cy == 0);

        e.EmulateOpcode(0xd2, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JNC (cy=1)")
    {
        // set cy = 1
        e.EmulateOpcode(0x37);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().cy == 1);

        e.EmulateOpcode(0xd2, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JZ (z=0)")
    {
        REQUIRE(e.GetFlags().z == 0);

        e.EmulateOpcode(0xca, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JZ (z=1)")
    {
        // set z = 1
        // A = 0x22 - 0x22 = 0x00
        e.EmulateOpcode(0x3e, 0x22);
        e.EmulateOpcode(0x06, 0x22);
        e.EmulateOpcode(0x90);

        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().z == 1);

        e.EmulateOpcode(0xca, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JNZ (z=0)")
    {
        REQUIRE(e.GetFlags().z == 0);

        e.EmulateOpcode(0xc2, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JNZ (z=1)")
    {
        // set z = 1
        // A = 0x22 - 0x22 = 0x00
        e.EmulateOpcode(0x3e, 0x22);
        e.EmulateOpcode(0x06, 0x22);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().z == 1);

        e.EmulateOpcode(0xc2, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JM (s=0)")
    {
        REQUIRE(e.GetFlags().s == 0);

        e.EmulateOpcode(0xfa, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JM (s=1)")
    {
        // set s = 1
        // A = 0xff - 0x11 = 0xee
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x06, 0x11);
        e.EmulateOpcode(0x90);

        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().s == 1);

        e.EmulateOpcode(0xfa, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JP (s=0)")
    {
        REQUIRE(e.GetFlags().s == 0);

        e.EmulateOpcode(0xf2, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JP (s=1)")
    {
        // set s = 1
        // A = 0xff - 0x11 = 0xee
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x06, 0x11);
        e.EmulateOpcode(0x90);
        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().s == 1);

        e.EmulateOpcode(0xf2, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JPE (p=0)")
    {
        REQUIRE(e.GetFlags().p == 0);

        e.EmulateOpcode(0xea, 0x34, 0x12);

        // No jump
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("JPE (p=1)")
    {
        // set p = 1: A-- -> 0xff (even parity)
        e.EmulateOpcode(0x3d);

        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().p == 1);

        e.EmulateOpcode(0xea, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JPO (p=0)")
    {
        REQUIRE(e.GetFlags().p == 0);

        e.EmulateOpcode(0xe2, 0x34, 0x12);

        // Jump
        CHECK(e.GetPC() == 0x1234);
    }
    SECTION("JPO (p=1)")
    {
        // set p = 1: A-- -> 0xff (even parity)
        e.EmulateOpcode(0x3d);

        pc_before = e.GetPC();

        REQUIRE(e.GetFlags().p == 1);

        e.EmulateOpcode(0xe2, 0x34, 0x12);

        // No jump
    }
}

TEST_CASE("Restart", "[stack][subroutine][restart][opcode]")
{
    Emulator e;
    e.AllocateMemory(0x3000);

    // Increase PC to 0x0102
    for (int i = 0; i < 0x0102; i++)
        e.EmulateOpcode(0x00);

    uint16_t pc_before = e.GetPC();
    REQUIRE(pc_before == 0x0102);

    uint8_t ret_high = (pc_before + 1) >> 8;
    uint8_t ret_low = (pc_before + 1) & 0x00ff;

    uint16_t sp_before = 0x2500;
    e.SetSP(sp_before);

    SECTION("RST 0")
    {
        e.EmulateOpcode(0xc7);

        CHECK(e.GetPC() == 0x0000);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 1")
    {
        e.EmulateOpcode(0xcf);

        CHECK(e.GetPC() == 0x0008);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 2")
    {
        e.EmulateOpcode(0xd7);

        CHECK(e.GetPC() == 0x0010);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 3")
    {
        e.EmulateOpcode(0xdf);

        CHECK(e.GetPC() == 0x0018);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 4")
    {
        e.EmulateOpcode(0xe7);

        CHECK(e.GetPC() == 0x0020);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 5")
    {
        e.EmulateOpcode(0xef);

        CHECK(e.GetPC() == 0x0028);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 6")
    {
        e.EmulateOpcode(0xf7);

        CHECK(e.GetPC() == 0x0030);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
    SECTION("RST 7")
    {
        e.EmulateOpcode(0xff);

        CHECK(e.GetPC() == 0x0038);
        CHECK(e.ReadFromMem(sp_before - 1) == ret_high);
        CHECK(e.ReadFromMem(sp_before - 2) == ret_low);
        CHECK(e.GetSP() == sp_before - 2);
    }
}

TEST_CASE("Rotate", "[opcode][rotate]")
{
    Emulator e;

    // MVI A
    e.EmulateOpcode(0x3e, 0x7e);

    // Set carry
    e.EmulateOpcode(0x37);

    // before
    // A = 7e = 0111 1110
    // cy = 1
    REQUIRE(e.GetRegisters().A == 0x7e);
    REQUIRE(e.GetFlags().cy == 1);

    uint16_t pc_before = e.GetPC();

    SECTION("RLC - rotate A left")
    {
        e.EmulateOpcode(0x07);

        // A: 7e = 0111 1110 -> 1111 1100 = fc
        // cy: set to 0
        CHECK(e.GetRegisters().A == 0xfc);
        CHECK(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetFlags().cy == 0);

        // MVI A
        e.EmulateOpcode(0x3e, 0xf2);

        // RLC
        e.EmulateOpcode(0x07);

        // A: f2 = 1111 0010 -> 1110 0101 = e5
        // cy: set to 1
        CHECK(e.GetRegisters().A == 0xe5);
        CHECK(e.GetFlags().cy == 1);
    }
    SECTION("RRC - rotate A right")
    {
        e.EmulateOpcode(0x0f);

        // A: 7e = 0111 1110 -> 0011 1111 = 3f
        // cy: set to 0
        CHECK(e.GetRegisters().A == 0x3f);
        CHECK(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetFlags().cy == 0);

        // MVI A
        e.EmulateOpcode(0x3e, 0xf2);

        // RRC
        e.EmulateOpcode(0x0f);

        // A: f2 = 1111 0010 -> 0111 1001 = 79
        // cy: set to 0
        CHECK(e.GetRegisters().A == 0x79);
        CHECK(e.GetFlags().cy == 0);
    }
    SECTION("RAL - rotate A left through carry")
    {
        e.EmulateOpcode(0x17);

        // A: 7e = 0111 1110 -> 1111 1101 = fd
        // cy: set to 0
        CHECK(e.GetRegisters().A == 0xfd);
        CHECK(e.GetPC() == pc_before + 1);
        REQUIRE(e.GetFlags().cy == 0);

        // MVI A
        e.EmulateOpcode(0x3e, 0xb5);

        // RAL
        e.EmulateOpcode(0x17);

        // A: b5 = 1011 0101 -> 0110 1010= 6a
        // cy: set to 1
        CHECK(e.GetRegisters().A == 0x6a);
        CHECK(e.GetFlags().cy == 1);
    }
    SECTION("RAR - rotate A right through carry")
    {
        e.EmulateOpcode(0x1f);

        // A: 7e = 0111 1110 -> 1011 1111 = bf
        // cy: set to 0
        CHECK(e.GetRegisters().A == 0xbf);
        CHECK(e.GetPC() == pc_before + 1);
        CHECK(e.GetFlags().cy == 0);

        // MVI A
        e.EmulateOpcode(0x3e, 0x6a);

        // Set Carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);

        // RAR
        e.EmulateOpcode(0x1f);

        // A: 6a = 0110 1010 -> 1011 0101 = b5
        // cy: set to 0
        CHECK(e.GetRegisters().A == 0xb5);
        CHECK(e.GetFlags().cy == 0);
    }
}
