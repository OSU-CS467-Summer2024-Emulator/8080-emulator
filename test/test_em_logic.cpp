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
        pc_before += 1;

        // MVI B = 11110000
        e.EmulateOpcode(0x06, 0xf0);
        REQUIRE(e.GetRegisters().B == 0xf0);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

        // AND A with D, store in A
        // 10101010 & 00000000 = 00000000 (0x00)
        e.EmulateOpcode(0xa2);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("ANA E")
    {
        // MVI E = 10101010
        e.EmulateOpcode(0x1e, 0xaa);
        REQUIRE(e.GetRegisters().E == 0xaa);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

        // AND A with L, store in A
        // 10101010 & 01010101 = 00000000 (0x00)
        e.EmulateOpcode(0xa5);
        CHECK(e.GetRegisters().A == 0x00);
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
        pc_before += 2;

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

        // MVI M = 11010011
        e.EmulateOpcode(0x36, 0xd3);
        REQUIRE(e.ReadFromHL() == 0xd3);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 1;

        // MVI B = 11110000
        e.EmulateOpcode(0x06, 0xf0);
        REQUIRE(e.GetRegisters().B == 0xf0);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

        // XOR A with E, store in A
        // 10101010 ^ 10101010 = 00000000 (0x00)
        e.EmulateOpcode(0xab);
        CHECK(e.GetRegisters().A == 0x00);
        CHECK(e.GetPC() == pc_before + 1);
    }

    SECTION("XRA H")
    {
        // MVI H = 00001010
        e.EmulateOpcode(0x26, 0x0a);
        REQUIRE(e.GetRegisters().H == 0x0a);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

        // MVI M = 11010011
        e.EmulateOpcode(0x36, 0xd3);
        REQUIRE(e.ReadFromHL() == 0xd3);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 1;

        // MVI B = 11110000
        e.EmulateOpcode(0x06, 0xf0);
        REQUIRE(e.GetRegisters().B == 0xf0);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

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
        pc_before += 2;

        // MVI L
        e.EmulateOpcode(0x2e, 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

        // MVI M = 11010011
        e.EmulateOpcode(0x36, 0xd3);
        REQUIRE(e.ReadFromHL() == 0xd3);
        REQUIRE(e.GetPC() == pc_before + 2);
        pc_before += 2;

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