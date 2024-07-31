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

TEST_CASE("MOV B", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV B, B")
    {
        e.EmulateOpcode(0x40);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, C")
    {
        // MVI into registers B and C
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x41);
        CHECK(e.GetRegisters().B == 0xcc);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, D")
    {
        // MVI into registers B and D
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x42);
        CHECK(e.GetRegisters().B == 0xdd);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, E")
    {
        // MVI into registers B and E
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x43);
        CHECK(e.GetRegisters().B == 0xee);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, H")
    {
        // MVI into registers B and H
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x26, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x44);
        CHECK(e.GetRegisters().B == 0xff);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, L")
    {
        // MVI into registers B and L
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x2e, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x45);
        CHECK(e.GetRegisters().B == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers B, H, and L
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x26, 0x25);
        e.EmulateOpcode(0x2e, 0x04);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0x11);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x46);
        CHECK(e.GetRegisters().B == 0x11);
        CHECK(e.GetRegisters().H == 0x25);
        CHECK(e.GetRegisters().L == 0x04);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV B, A")
    {
        // MVI into registers B and A
        e.EmulateOpcode(0x06, 0xbb);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x47);
        CHECK(e.GetRegisters().B == 0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV C", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV C, B")
    {
        // MVI into registers C and B
        e.EmulateOpcode(0x0e, 0xcc);
        e.EmulateOpcode(0x06, 0x12);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x48);
        CHECK(e.GetRegisters().C == 0x12);
        CHECK(e.GetRegisters().B == 0x12);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, C")
    {
        e.EmulateOpcode(0x49);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, D")
    {
        // MVI into registers C and D
        e.EmulateOpcode(0x0e, 0x01);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x4a);
        CHECK(e.GetRegisters().C == 0xdd);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, E")
    {
        // MVI into registers C and E
        e.EmulateOpcode(0x0e, 0xcc);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x4b);
        CHECK(e.GetRegisters().C == 0xee);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, H")
    {
        // MVI into registers C and H
        e.EmulateOpcode(0x0e, 0xcc);
        e.EmulateOpcode(0x26, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x4c);
        CHECK(e.GetRegisters().C == 0xff);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, L")
    {
        // MVI into registers B and L
        e.EmulateOpcode(0x0e, 0xcc);
        e.EmulateOpcode(0x2e, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x4d);
        CHECK(e.GetRegisters().C == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers C, H, and L
        e.EmulateOpcode(0x0e, 0xcc);
        e.EmulateOpcode(0x26, 0x29);
        e.EmulateOpcode(0x2e, 0x99);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x4e);
        CHECK(e.GetRegisters().C == 0xff);
        CHECK(e.GetRegisters().H == 0x29);
        CHECK(e.GetRegisters().L == 0x99);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV C, A")
    {
        // MVI into registers C and A
        e.EmulateOpcode(0x0e, 0xcc);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x4f);
        CHECK(e.GetRegisters().C == 0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV D", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV D, B")
    {
        // MVI into registers D and B
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x06, 0xbb);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x50);
        CHECK(e.GetRegisters().D == 0xbb);
        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, C")
    {
        // MVI into registers D and C
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x51);
        CHECK(e.GetRegisters().D == 0xcc);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, D")
    {
        e.EmulateOpcode(0x52);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, E")
    {
        // MVI into registers D and E
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x53);
        CHECK(e.GetRegisters().D == 0xee);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, H")
    {
        // MVI into registers D and H
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x26, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x54);
        CHECK(e.GetRegisters().D == 0xff);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, L")
    {
        // MVI into registers D and L
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x2e, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x55);
        CHECK(e.GetRegisters().D == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers D, H, and L
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x26, 0x20);
        e.EmulateOpcode(0x2e, 0x00);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0x01);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x56);
        CHECK(e.GetRegisters().D == 0x01);
        CHECK(e.GetRegisters().H == 0x20);
        CHECK(e.GetRegisters().L == 0x00);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV D, A")
    {
        // MVI into registers D and A
        e.EmulateOpcode(0x16, 0xdd);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x57);
        CHECK(e.GetRegisters().D == 0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV E", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV E, B")
    {
        // MVI into registers E and B
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x06, 0xbb);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x58);
        CHECK(e.GetRegisters().E == 0xbb);
        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, C")
    {
        // MVI into registers E and C
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x59);
        CHECK(e.GetRegisters().E == 0xcc);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, D")
    {
        // MVI into registers E and D
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x5a);
        CHECK(e.GetRegisters().E == 0xdd);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, E")
    {
        e.EmulateOpcode(0x5b);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, H")
    {
        // MVI into registers E and H
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x26, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x5c);
        CHECK(e.GetRegisters().E == 0xff);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, L")
    {
        // MVI into registers E and L
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x2e, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x5d);
        CHECK(e.GetRegisters().E == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers E, H, and L
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x26, 0x25);
        e.EmulateOpcode(0x2e, 0x55);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0x56);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x5e);
        CHECK(e.GetRegisters().E == 0x56);
        CHECK(e.GetRegisters().H == 0x25);
        CHECK(e.GetRegisters().L == 0x55);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV E, A")
    {
        // MVI into registers E and A
        e.EmulateOpcode(0x1e, 0xee);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x5f);
        CHECK(e.GetRegisters().E == 0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV H", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV H, B")
    {
        // MVI into registers H and B
        e.EmulateOpcode(0x26, 0xff);
        e.EmulateOpcode(0x06, 0xbb);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x60);
        CHECK(e.GetRegisters().H == 0xbb);
        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, C")
    {
        // MVI into registers H and C
        e.EmulateOpcode(0x26, 0xf1);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x61);
        CHECK(e.GetRegisters().H == 0xcc);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, D")
    {
        // MVI into registers H and D
        e.EmulateOpcode(0x26, 0xf2);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x62);
        CHECK(e.GetRegisters().H == 0xdd);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, E")
    {
        // MVI into registers H and E
        e.EmulateOpcode(0x26, 0xf2);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x63);
        CHECK(e.GetRegisters().H == 0xee);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, H")
    {
        e.EmulateOpcode(0x64);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, L")
    {
        // MVI into registers H and L
        e.EmulateOpcode(0x26, 0xff);
        e.EmulateOpcode(0x2e, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x65);
        CHECK(e.GetRegisters().H == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers H and L
        e.EmulateOpcode(0x26, 0x21);
        e.EmulateOpcode(0x2e, 0x06);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0x04);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x66);
        CHECK(e.GetRegisters().H == 0x04);
        CHECK(e.GetRegisters().L == 0x06);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV H, A")
    {
        // MVI into registers H and A
        e.EmulateOpcode(0x26, 0xee);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x67);
        CHECK(e.GetRegisters().H == 0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV L", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV L, B")
    {
        // MVI into registers L and B
        e.EmulateOpcode(0x2e, 0xff);
        e.EmulateOpcode(0x06, 0xbb);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x68);
        CHECK(e.GetRegisters().L == 0xbb);
        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, C")
    {
        // MVI into registers L and C
        e.EmulateOpcode(0x2e, 0xf1);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x69);
        CHECK(e.GetRegisters().L == 0xcc);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, D")
    {
        // MVI into registers L and D
        e.EmulateOpcode(0x2e, 0xf2);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x6a);
        CHECK(e.GetRegisters().L == 0xdd);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, E")
    {
        // MVI into registers L and E
        e.EmulateOpcode(0x2e, 0xf2);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x6b);
        CHECK(e.GetRegisters().L == 0xee);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, H")
    {
        // MVI into registers L and H
        e.EmulateOpcode(0x2e, 0xf8);
        e.EmulateOpcode(0x26, 0x08);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x6c);
        CHECK(e.GetRegisters().L == 0x08);
        CHECK(e.GetRegisters().H == 0x08);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, L")
    {
        e.EmulateOpcode(0x6d);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers H and L
        e.EmulateOpcode(0x26, 0x28);
        e.EmulateOpcode(0x2e, 0x5f);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0xd2);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x6e);
        CHECK(e.GetRegisters().H == 0x28);
        CHECK(e.GetRegisters().L == 0xd2);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV L, A")
    {
        // MVI into registers L and A
        e.EmulateOpcode(0x2e, 0xee);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x6f);
        CHECK(e.GetRegisters().L == 0xaa);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV M", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    // Allocate memory for writing
    e.AllocateMemory(0x3000);

    SECTION("MOV M, B")
    {
        // MVI into registers H, L, and B
        e.EmulateOpcode(0x26, 0x28);
        e.EmulateOpcode(0x2e, 0x5f);
        e.EmulateOpcode(0x06, 0xbb);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x70);
        CHECK(e.GetRegisters().H == 0x28);
        CHECK(e.GetRegisters().L == 0x5f);
        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.ReadFromHL() == 0xbb);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV M, C")
    {
        // MVI into registers H, L, and C
        e.EmulateOpcode(0x26, 0x20);
        e.EmulateOpcode(0x2e, 0x04);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x71);
        CHECK(e.GetRegisters().H == 0x20);
        CHECK(e.GetRegisters().L == 0x04);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.ReadFromHL() == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV M, D")
    {
        // MVI into registers H, L, and D
        e.EmulateOpcode(0x26, 0x20);
        e.EmulateOpcode(0x2e, 0x04);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x72);
        CHECK(e.GetRegisters().H == 0x20);
        CHECK(e.GetRegisters().L == 0x04);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.ReadFromHL() == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV M, E")
    {
        // MVI into registers H, L, and E
        e.EmulateOpcode(0x26, 0x20);
        e.EmulateOpcode(0x2e, 0x04);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x73);
        CHECK(e.GetRegisters().H == 0x20);
        CHECK(e.GetRegisters().L == 0x04);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.ReadFromHL() == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV M, H")
    {
        // MVI into registers H and L
        e.EmulateOpcode(0x26, 0x20);
        e.EmulateOpcode(0x2e, 0x04);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x74);
        CHECK(e.GetRegisters().H == 0x20);
        CHECK(e.GetRegisters().L == 0x04);
        CHECK(e.ReadFromHL() == 0x20);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV M, L")
    {
        // MVI into registers H and L
        e.EmulateOpcode(0x26, 0x20);
        e.EmulateOpcode(0x2e, 0x04);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x75);
        CHECK(e.GetRegisters().H == 0x20);
        CHECK(e.GetRegisters().L == 0x04);
        CHECK(e.ReadFromHL() == 0x04);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV M, A")
    {
        // MVI into registers H, L, and A
        e.EmulateOpcode(0x26, 0x28);
        e.EmulateOpcode(0x2e, 0x5f);
        e.EmulateOpcode(0x3e, 0xaa);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x77);
        CHECK(e.GetRegisters().H == 0x28);
        CHECK(e.GetRegisters().L == 0x5f);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.ReadFromHL() == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("MOV A", "[opcode][store][load]")
{
    Emulator e;
    int pc = e.GetPC();
    Registers registers_before = e.GetRegisters();

    SECTION("MOV A, B")
    {
        // MVI into registers A and B
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x06, 0xbb);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x78);
        CHECK(e.GetRegisters().A == 0xbb);
        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, C")
    {
        // MVI into registers A and C
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x0e, 0xcc);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x79);
        CHECK(e.GetRegisters().A == 0xcc);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, D")
    {
        // MVI into registers A and D
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x16, 0xdd);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x7a);
        CHECK(e.GetRegisters().A == 0xdd);
        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, E")
    {
        // MVI into registers A and E
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x1e, 0xee);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x7b);
        CHECK(e.GetRegisters().A == 0xee);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, H")
    {
        // MVI into registers A and H
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x26, 0x08);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x7c);
        CHECK(e.GetRegisters().A == 0x08);
        CHECK(e.GetRegisters().H == 0x08);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, L")
    {
        // MVI into registers A and L
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x2e, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x7d);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetRegisters().L == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, M")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers A, H and L
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x26, 0x28);
        e.EmulateOpcode(0x2e, 0x5f);

        // Write value to addresses stored by M (HL)
        e.EmulateOpcode(0x36, 0xff);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x7e);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetRegisters().H == 0x28);
        CHECK(e.GetRegisters().L == 0x5f);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("MOV A, A")
    {
        e.EmulateOpcode(0x7f);
        CHECK(e.GetRegisters() == registers_before);
        CHECK(e.GetPC() == pc + 1);
    }
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
    SECTION("MVI A - Max value")
    {
        e.EmulateOpcode(0x3e, 0xff);
        CHECK((uint8_t)e.GetRegisters().A == 0xff);
        CHECK(e.GetPC() == pc + 2);
    }
}

TEST_CASE("STAX", "[opcode][store]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("STAX B")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers A, B and C
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x06, 0x25);
        e.EmulateOpcode(0x0e, 0x01);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x02);
        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetRegisters().B == 0x25);
        CHECK(e.GetRegisters().C == 0x01);
        CHECK(e.ReadFromMem(0x2501) == 0xaa);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("STAX D")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x4000);

        // MVI into registers A, D and E
        e.EmulateOpcode(0x3e, 0xff);
        e.EmulateOpcode(0x16, 0x39);
        e.EmulateOpcode(0x1e, 0x99);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x12);
        CHECK(e.GetRegisters().A == 0xff);
        CHECK(e.GetRegisters().D == 0x39);
        CHECK(e.GetRegisters().E == 0x99);
        CHECK(e.ReadFromMem(0x3999) == 0xff);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("LDAX", "[opcode][load]")
{
    Emulator e;
    int pc = e.GetPC();

    SECTION("LDAX B")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x3000);

        // MVI into registers A, B and C
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x06, 0x25);
        e.EmulateOpcode(0x0e, 0x01);

        // Write value into memory at address stored by B and C
        e.WriteToMem(0x2501, 0xc3);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x0a);
        CHECK(e.GetRegisters().A == 0xc3);
        CHECK(e.GetRegisters().B == 0x25);
        CHECK(e.GetRegisters().C == 0x01);
        CHECK(e.ReadFromMem(0x2501) == 0xc3);
        CHECK(e.GetPC() == pc + 1);
    }

    SECTION("LDAX D")
    {
        // Allocate memory for writing
        e.AllocateMemory(0x4000);

        // MVI into registers A, D and E
        e.EmulateOpcode(0x3e, 0xaa);
        e.EmulateOpcode(0x16, 0x34);
        e.EmulateOpcode(0x1e, 0x10);

        // Write value into memory at address stored by D and E
        e.WriteToMem(0x3410, 0xe7);

        // update PC prior to tested instruction
        pc = e.GetPC();

        // Test MOV instruction
        e.EmulateOpcode(0x1a);
        CHECK(e.GetRegisters().A == 0xe7);
        CHECK(e.GetRegisters().D == 0x34);
        CHECK(e.GetRegisters().E == 0x10);
        CHECK(e.ReadFromMem(0x3410) == 0xe7);
        CHECK(e.GetPC() == pc + 1);
    }
}

TEST_CASE("LXI - load immediate register pair", "[opcode][load][move][immediate]")
{
    Emulator e;

    uint16_t pc_before = e.GetPC();
    SECTION("LXI B")
    {
        REQUIRE(e.GetRegisters().B == 0x00);
        REQUIRE(e.GetRegisters().C == 0x00);

        // LXI B
        e.EmulateOpcode(0x01, 0xcc, 0xbb);

        CHECK(e.GetRegisters().B == 0xbb);
        CHECK(e.GetRegisters().C == 0xcc);
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("LXI D")
    {
        REQUIRE(e.GetRegisters().D == 0x00);
        REQUIRE(e.GetRegisters().E == 0x00);

        // LXI D
        e.EmulateOpcode(0x11, 0xee, 0xdd);

        CHECK(e.GetRegisters().D == 0xdd);
        CHECK(e.GetRegisters().E == 0xee);
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("LXI H")
    {
        REQUIRE(e.GetRegisters().H == 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);

        // LXI H
        e.EmulateOpcode(0x21, 0x34, 0x12);

        CHECK(e.GetRegisters().H == 0x12);
        CHECK(e.GetRegisters().L == 0x34);
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("LXI SP")
    {
        REQUIRE(e.GetSP() == 0x00);

        // LXI SP
        e.EmulateOpcode(0x31, 0x34, 0x12);

        CHECK(e.GetSP() == 0x1234);
        CHECK(e.GetPC() == pc_before + 3);
    }
}

TEST_CASE("Direct Addressing Instructions", "[opcode][store][load]")
{
    Emulator e;
    e.AllocateMemory(0x3000);

    uint16_t pc_before = e.GetPC();

    SECTION("STA - store accumulator direct")
    {
        // MVI A
        e.EmulateOpcode(0x3e, 0xaa);

        REQUIRE(e.GetRegisters().A == 0xaa);
        REQUIRE(e.ReadFromMem(0x2500) == 0x00);

        pc_before = e.GetPC();

        // STA
        e.EmulateOpcode(0x32, 0x00, 0x25);

        CHECK(e.ReadFromMem(0x2500) == 0xaa);
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("LDA - load accumulator direct")
    {
        e.WriteToMem(0x2500, 0xaa);

        REQUIRE(e.GetRegisters().A == 0x00);
        REQUIRE(e.ReadFromMem(0x2500) == 0xaa);

        // LDA
        e.EmulateOpcode(0x3a, 0x00, 0x25);

        CHECK(e.GetRegisters().A == 0xaa);
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("SHLD - store HL direct")
    {
        // MVI H and L
        e.EmulateOpcode(0x26, 0x88);
        e.EmulateOpcode(0x2e, 0x99);

        REQUIRE(e.GetRegisters().H == 0x88);
        REQUIRE(e.GetRegisters().L == 0x99);
        REQUIRE(e.ReadFromMem(0x2500) == 0x00);
        REQUIRE(e.ReadFromMem(0x2501) == 0x00);

        pc_before = e.GetPC();

        // SHLD
        e.EmulateOpcode(0x22, 0x00, 0x25);

        CHECK(e.ReadFromMem(0x2500) == 0x99);
        CHECK(e.ReadFromMem(0x2501) == 0x88);
        CHECK(e.GetPC() == pc_before + 3);
    }
    SECTION("LHLD - load HL direct")
    {
        // MVI H and L
        e.WriteToMem(0x2500, 0x11);
        e.WriteToMem(0x2501, 0x22);

        REQUIRE(e.GetRegisters().H == 0x00);
        REQUIRE(e.GetRegisters().L == 0x00);
        REQUIRE(e.ReadFromMem(0x2500) == 0x11);
        REQUIRE(e.ReadFromMem(0x2501) == 0x22);

        // LHLD
        e.EmulateOpcode(0x2a, 0x00, 0x25);

        CHECK(e.GetRegisters().H == 0x22);
        CHECK(e.GetRegisters().L == 0x11);
        CHECK(e.GetPC() == pc_before + 3);
    }
}

TEST_CASE("XCHG - exchange registers", "[opcode]")
{
    Emulator e;

    // MVI D, E, H, and L
    e.EmulateOpcode(0x16, 0xdd);
    e.EmulateOpcode(0x1e, 0xee);
    e.EmulateOpcode(0x26, 0x11);
    e.EmulateOpcode(0x2e, 0xff);

    uint16_t pc_before = e.GetPC();

    REQUIRE(e.GetRegisters().D == 0xdd);
    REQUIRE(e.GetRegisters().E == 0xee);
    REQUIRE(e.GetRegisters().H == 0x11);
    REQUIRE(e.GetRegisters().L == 0xff);

    // XCHG
    e.EmulateOpcode(0xeb);

    CHECK(e.GetRegisters().D == 0x11);
    CHECK(e.GetRegisters().E == 0xff);
    CHECK(e.GetRegisters().H == 0xdd);
    CHECK(e.GetRegisters().L == 0xee);
    CHECK(e.GetPC() == pc_before + 1);
}

TEST_CASE("XTHL - exchange stack", "[opcode][stack]")
{
    Emulator e;
    e.AllocateMemory(0x3000);
    e.SetSP(0x2500);

    // MVI H and L
    e.EmulateOpcode(0x26, 0xee);
    e.EmulateOpcode(0x2e, 0xff);

    e.Push(0x11, 0x22);

    uint16_t pc_before = e.GetPC();
    uint16_t sp_before = e.GetSP();

    REQUIRE(e.GetRegisters().H == 0xee);
    REQUIRE(e.GetRegisters().L == 0xff);
    REQUIRE(e.ReadFromMem(0x24ff) == 0x11);
    REQUIRE(e.ReadFromMem(0x24fe) == 0x22);

    // XTHL
    e.EmulateOpcode(0xe3);

    CHECK(e.GetRegisters().H == 0x11);
    CHECK(e.GetRegisters().L == 0x22);
    CHECK(e.ReadFromMem(0x24ff) == 0xee);
    CHECK(e.ReadFromMem(0x24fe) == 0xff);
    CHECK(e.GetPC() == pc_before + 1);
    CHECK(e.GetSP() == sp_before);
}
