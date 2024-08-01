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

TEST_CASE("AND Register with A", "[opcode][and]")
{
    Emulator e;

    // MVI A: 0xaa = 10101010
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    int pc_before = e.GetPC();

    SECTION("ANA B - Reset Carry")
    {
        // Set carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        pc_before = e.GetPC();

        // MVI B = 11110000
        e.EmulateOpcode(0x06, 0xf0);
        REQUIRE(e.GetRegisters().B == 0xf0);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with B, store in A
        // 10101010 & 11110000 = 10100000 (0xa0)
        e.EmulateOpcode(0xa0);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xa0);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA B - No Carry")
    {
        // MVI B = 11110001
        e.EmulateOpcode(0x06, 0xf1);
        REQUIRE(e.GetRegisters().B == 0xf1);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with B, store in A
        // 10101010 & 11110001 = 10100000 (0xa0)
        e.EmulateOpcode(0xa0);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xa0);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA C")
    {
        // MVI C = 11110010
        e.EmulateOpcode(0x0e, 0xf2);
        REQUIRE(e.GetRegisters().C == 0xf2);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with C, store in A
        // 10101010 & 11110010 = 10100010 (0xa2)
        e.EmulateOpcode(0xa1);
        CHECK(e.GetRegisters().A == 0xa2);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA D")
    {
        // MVI D = 00000000
        e.EmulateOpcode(0x16, 0x00);
        REQUIRE(e.GetRegisters().D == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with D, store in A
        // 10101010 & 00000000 = 00000000 (0x00)
        e.EmulateOpcode(0xa2);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA E")
    {
        // MVI E = 10101010
        e.EmulateOpcode(0x1e, 0xaa);
        REQUIRE(e.GetRegisters().E == 0xaa);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with E, store in A
        // 10101010 & 10101010 = 10101010 (0xaa)
        e.EmulateOpcode(0xa3);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA H")
    {
        // MVI H = 00001010
        e.EmulateOpcode(0x26, 0x0a);
        REQUIRE(e.GetRegisters().H == 0x0a);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with H, store in A
        // 10101010 & 00001010 = 00001010 (0x0a)
        e.EmulateOpcode(0xa4);
        CHECK(e.GetRegisters().A == 0x0a);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA L")
    {
        // MVI L = 01010101
        e.EmulateOpcode(0x2e, 0x55);
        REQUIRE(e.GetRegisters().L == 0x55);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with L, store in A
        // 10101010 & 01010101 = 00000000 (0x00)
        e.EmulateOpcode(0xa5);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA M")
    {
        // Alocate Memory
        e.AllocateMemory(0x3000);

        // MVI H
        e.EmulateOpcode(0x26, 0x25);
        REQUIRE(e.GetRegisters().H == 0x25);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI M = 11010011
        e.EmulateOpcode(0x36, 0xd3);
        REQUIRE(e.ReadFromHL() == 0xd3);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // AND A with M, store in A
        // 10101010 & 11010011 = 10000010 (0x82)
        e.EmulateOpcode(0xa6);
        CHECK(e.GetRegisters().A == 0x82);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA A")
    {
        // AND A with A, store in A
        // 10101010 & 10101010 = 10101010 (0xaa)
        e.EmulateOpcode(0xa7);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }
}

TEST_CASE("XOR Register with A", "[opcode][xor]")
{
    Emulator e;

    // MVI A: 0xaa = 10101010
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    int pc_before = e.GetPC();

    SECTION("XRA B - Reset Carry")
    {
        // Set carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        pc_before = e.GetPC();

        // MVI B = 11110000
        e.EmulateOpcode(0x06, 0xf0);
        REQUIRE(e.GetRegisters().B == 0xf0);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with B, store in A
        // 10101010 ^ 11110000 = 01011010 (0x5a)
        e.EmulateOpcode(0xa8);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0x5a);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA B - No Carry")
    {
        // MVI B = 11110001
        e.EmulateOpcode(0x06, 0xf1);
        REQUIRE(e.GetRegisters().B == 0xf1);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with B, store in A
        // 10101010 ^ 11110001 = 01011011 (0x5b)
        e.EmulateOpcode(0xa8);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0x5b);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA C")
    {
        // MVI C = 11110010
        e.EmulateOpcode(0x0e, 0xf2);
        REQUIRE(e.GetRegisters().C == 0xf2);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with C, store in A
        // 10101010 ^ 11110010 = 01011000 (0x58)
        e.EmulateOpcode(0xa9);
        CHECK(e.GetRegisters().A == 0x58);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA D")
    {
        // MVI D = 00000000
        e.EmulateOpcode(0x16, 0x00);
        REQUIRE(e.GetRegisters().D == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with D, store in A
        // 10101010 ^ 00000000 = 10101010 (0xaa)
        e.EmulateOpcode(0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA E")
    {
        // MVI E = 10101010
        e.EmulateOpcode(0x1e, 0xaa);
        REQUIRE(e.GetRegisters().E == 0xaa);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with E, store in A
        // 10101010 ^ 10101010 = 00000000 (0x00)
        e.EmulateOpcode(0xab);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA H")
    {
        // MVI H = 00001010
        e.EmulateOpcode(0x26, 0x0a);
        REQUIRE(e.GetRegisters().H == 0x0a);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with H, store in A
        // 10101010 ^ 00001010 = 10100000 (0xa0)
        e.EmulateOpcode(0xac);
        CHECK(e.GetRegisters().A == 0xa0);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA L")
    {
        // MVI L = 01010101
        e.EmulateOpcode(0x2e, 0x55);
        REQUIRE(e.GetRegisters().L == 0x55);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with L, store in A
        // 10101010 ^ 01010101 = 11111111 (0xff)
        e.EmulateOpcode(0xad);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA M")
    {
        // Alocate Memory
        e.AllocateMemory(0x3000);

        // MVI H
        e.EmulateOpcode(0x26, 0x25);
        REQUIRE(e.GetRegisters().H == 0x25);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI M = 11010011
        e.EmulateOpcode(0x36, 0xd3);
        REQUIRE(e.ReadFromHL() == 0xd3);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // XOR A with M, store in A
        // 10101010 ^ 11010011 = 01111001 (0x82)
        e.EmulateOpcode(0xae);
        CHECK(e.GetRegisters().A == 0x79);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA A")
    {
        // XOR A with A, store in A
        // 10101010 ^ 10101010 = 00000000 (0x00)
        e.EmulateOpcode(0xaf);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetPC() == pc_before + 1);
    }
}

TEST_CASE("OR Register with A", "[opcode][xor]")
{
    Emulator e;

    // MVI A: 0xaa = 10101010
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    int pc_before = e.GetPC();

    SECTION("ORA B - Reset Carry")
    {
        // Set carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        pc_before = e.GetPC();

        // MVI B = 11110000
        e.EmulateOpcode(0x06, 0xf0);
        REQUIRE(e.GetRegisters().B == 0xf0);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with B, store in A
        // 10101010 | 11110000 = 11111010 (0xfa)
        e.EmulateOpcode(0xb0);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xfa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ORA B - No Carry")
    {
        // MVI B = 11110001
        e.EmulateOpcode(0x06, 0xf1);
        REQUIRE(e.GetRegisters().B == 0xf1);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with B, store in A
        // 10101010 | 11110001 = 11111011 (0xfb)
        e.EmulateOpcode(0xb0);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xfb);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ORA C")
    {
        // MVI C = 11110010
        e.EmulateOpcode(0x0e, 0xf2);
        REQUIRE(e.GetRegisters().C == 0xf2);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with C, store in A
        // 10101010 | 11110010 = 11111010 (0xfa)
        e.EmulateOpcode(0xb1);
        CHECK(e.GetRegisters().A == 0xfa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ORA D")
    {
        // MVI D = 00000000
        e.EmulateOpcode(0x16, 0x00);
        REQUIRE(e.GetRegisters().D == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with D, store in A
        // 10101010 | 00000000 = 10101010 (0xaa)
        e.EmulateOpcode(0xb2);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ORA E")
    {
        // MVI E = 10101010
        e.EmulateOpcode(0x1e, 0xaa);
        REQUIRE(e.GetRegisters().E == 0xaa);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with E, store in A
        // 10101010 | 10101010 = 10101010 (0xaa)
        e.EmulateOpcode(0xb3);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ORA H")
    {
        // MVI H = 00001010
        e.EmulateOpcode(0x26, 0x0a);
        REQUIRE(e.GetRegisters().H == 0x0a);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with H, store in A
        // 10101010 | 00001010 = 10101010 (0xaa)
        e.EmulateOpcode(0xb4);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA L")
    {
        // MVI L = 01010101
        e.EmulateOpcode(0x2e, 0x55);
        REQUIRE(e.GetRegisters().L == 0x55);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with L, store in A
        // 10101010 | 01010101 = 11111111 (0xff)
        e.EmulateOpcode(0xb5);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA M")
    {
        // Alocate Memory
        e.AllocateMemory(0x3000);

        // MVI H
        e.EmulateOpcode(0x26, 0x25);
        REQUIRE(e.GetRegisters().H == 0x25);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI M = 11010011
        e.EmulateOpcode(0x36, 0xd3);
        REQUIRE(e.ReadFromHL() == 0xd3);
        REQUIRE(e.GetPC() == pc_before + 2);
        
        pc_before = e.GetPC();

        // OR A with M, store in A
        // 10101010 | 11010011 = 11111011 (0xfb)
        e.EmulateOpcode(0xb6);
        CHECK(e.GetRegisters().A == 0xfb);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA A")
    {
        // OR A with A, store in A
        // 10101010 | 10101010 = 10101010 (0xaa)
        e.EmulateOpcode(0xb7);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 1);
    }
}

TEST_CASE("CMP Register with A", "[opcode][cmp]")
{
    Emulator e;

    // MVI A: 0xaa
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    Flags flags_before = e.GetFlags();
    int pc_before = e.GetPC();

    SECTION("CMP B - Zero Flag Set, A == B")
    {
        // MVI B
        e.EmulateOpcode(0x06, 0xaa);
        REQUIRE(e.GetRegisters().B == 0xaa);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // Make sure zero flag is not set
        REQUIRE(e.GetFlags().z == 0);

        // CMP: res = A - B
        e.EmulateOpcode(0xb8);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetFlags().s == 0);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP B - Carry and Sign Flags Set, A < B")
    {
        // MVI B
        e.EmulateOpcode(0x06, 0xab);
        REQUIRE(e.GetRegisters().B == 0xab);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // Make sure sign and carry flags not set
        REQUIRE(e.GetFlags().cy == 0);
        REQUIRE(e.GetFlags().s == 0);

        // CMP: res = A - B
        e.EmulateOpcode(0xb8);
        CHECK(e.GetFlags().z == 0);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetFlags().s == 1);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP B - No Flags Set, A > B")
    {
        // MVI B
        e.EmulateOpcode(0x06, 0xa9);
        REQUIRE(e.GetRegisters().B == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - B
        e.EmulateOpcode(0xb8);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP C")
    {
        // MVI C
        e.EmulateOpcode(0x0e, 0xa9);
        REQUIRE(e.GetRegisters().C == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - C
        e.EmulateOpcode(0xb9);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP D")
    {
        // MVI D
        e.EmulateOpcode(0x16, 0xa9);
        REQUIRE(e.GetRegisters().D == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - D
        e.EmulateOpcode(0xba);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP E")
    {
        // MVI E
        e.EmulateOpcode(0x1e, 0xa9);
        REQUIRE(e.GetRegisters().E == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - E
        e.EmulateOpcode(0xbb);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP H")
    {
        // MVI H
        e.EmulateOpcode(0x26, 0xa9);
        REQUIRE(e.GetRegisters().H == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - H
        e.EmulateOpcode(0xbc);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP L")
    {
        // MVI L
        e.EmulateOpcode(0x2e, 0xa9);
        REQUIRE(e.GetRegisters().L == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - L
        e.EmulateOpcode(0xbd);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP M")
    {
        // Allocate memory
        e.AllocateMemory(0x3000);

        // MVI H
        e.EmulateOpcode(0x26, 0x25);
        REQUIRE(e.GetRegisters().H == 0x25);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // MVI M
        e.EmulateOpcode(0x36, 0xa9);
        REQUIRE(e.ReadFromHL() == 0xa9);
        REQUIRE(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // CMP: res = A - M
        e.EmulateOpcode(0xbe);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("CMP A")
    {
        // CMP: res = A - A
        e.EmulateOpcode(0xbf);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetFlags().s == 0);
        CHECK(e.GetPC() == pc_before + 1);
    }
}

TEST_CASE("AND Immediate with A", "[opcode][and]")
{
    Emulator e;

    // MVI A = 10101010
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    int pc_before = e.GetPC();

    SECTION("ANI Immediate - Reset Carry")
    {
        // Set carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        pc_before = e.GetPC();

        // ANI A with Immediate, store in A
        // 10101010 & 10101010 = 10101010 (0xaa)
        e.EmulateOpcode(0xe6, 0xaa);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("ANI Immediate - Set 0")
    {
        // ANI A with Immediate, store in A
        // 10101010 & 00000000 = 00000000 (0x00)
        e.EmulateOpcode(0xe6, 0x00);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("ANI Immediate")
    {
        // ANI A with Immediate, store in A
        // 10101010 & 11110000 = 10100000 (0xa0)
        e.EmulateOpcode(0xe6, 0xf0);
        CHECK(e.GetRegisters().A == 0xa0);
        CHECK(e.GetPC() == pc_before + 2);
    }
}

TEST_CASE("XOR Immediate with A", "[opcode][xor]")
{
    Emulator e;

    // MVI A = 10101010
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    int pc_before = e.GetPC();

    SECTION("XRI Immediate - Reset Carry")
    {
        // Set carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        pc_before = e.GetPC();

        // XRI A with Immediate, store in A
        // 10101010 ^ 01010101 = 11111111 (0xff)
        e.EmulateOpcode(0xee, 0x55);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("XRI Immediate - Set 0")
    {
        // XRI A with Immediate, store in A
        // 10101010 ^ 10101010 = 00000000 (0x00)
        e.EmulateOpcode(0xee, 0xaa);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("XRI Immediate")
    {
        // XRI A with Immediate, store in A
        // 10101010 & 11110000 = 01011010 (0x5a)
        e.EmulateOpcode(0xee, 0xf0);
        CHECK(e.GetRegisters().A == 0x5a);
        CHECK(e.GetPC() == pc_before + 2);
    }
}

TEST_CASE("OR Immediate with A", "[opcode][or]")
{
    Emulator e;

    // MVI A = 10101010
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    int pc_before = e.GetPC();

    SECTION("OR Immediate - Reset Carry")
    {
        // Set carry
        e.EmulateOpcode(0x37);
        REQUIRE(e.GetFlags().cy == 1);
        REQUIRE(e.GetPC() == pc_before + 1);

        pc_before = e.GetPC();

        // OR A with Immediate, store in A
        // 10101010 | 01010101 = 11111111 (0xff)
        e.EmulateOpcode(0xf6, 0x55);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("OR Immediate - Set 0")
    {
        // MVI A = 00000000
        e.EmulateOpcode(0x3e, 0x00);
        REQUIRE(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == pc_before + 2);

        pc_before = e.GetPC();

        // OR A with Immediate, store in A
        // 00000000 | 00000000 = 00000000 (0x00)
        e.EmulateOpcode(0xf6, 0x00);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("OR Immediate")
    {
        // OR A with Immediate, store in A
        // 10101010 | 11110000 = 11111010 (0xfa)
        e.EmulateOpcode(0xf6, 0xf0);
        CHECK(e.GetRegisters().A == 0xfa);
        CHECK(e.GetPC() == pc_before + 2);
    }
}

TEST_CASE("CMP Immediate with A", "[opcode][cmp]")
{
    Emulator e;

    // MVI A: 0xaa
    e.EmulateOpcode(0x3e, 0xaa);
    REQUIRE(e.GetRegisters().A == 0xaa);

    Flags flags_before = e.GetFlags();
    int pc_before = e.GetPC();

    SECTION("CPI - Zero Flag Set, A == I")
    {
        // Make sure zero flag is not set
        REQUIRE(e.GetFlags().z == 0);

        // CMP: res = A - I
        e.EmulateOpcode(0xfe, 0xaa);
        CHECK(e.GetFlags().z == 1);
        CHECK(e.GetFlags().cy == 0);
        CHECK(e.GetFlags().s == 0);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("CPI - Carry and Sign Flags Set, A < I")
    {
        // Make sure sign and carry flags not set
        REQUIRE(e.GetFlags().cy == 0);
        REQUIRE(e.GetFlags().s == 0);

        // CMP: res = A - I
        e.EmulateOpcode(0xfe, 0xab);
        CHECK(e.GetFlags().z == 0);
        CHECK(e.GetFlags().cy == 1);
        CHECK(e.GetFlags().s == 1);
        CHECK(e.GetPC() == pc_before + 2);
    }

    SECTION("CPI - No Flags Set, A > I")
    {
        // CMP: res = A - I
        e.EmulateOpcode(0xfe, 0xa9);
        CHECK(e.GetFlags() == flags_before);
        CHECK(e.GetPC() == pc_before + 2);
    }
}
