#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include "emulator/emulator.hpp"
#include "disassembler/disassembler.hpp"

using namespace std;

// Constructor
Emulator::Emulator()
{
    pc = 0;
    sp = 0;
    memory = nullptr;
    mem_size = 0;
}

// Destructor
Emulator::~Emulator()
{
    // deallocate memory if necessary
    if (memory != nullptr)
    {
        delete[] memory;
    }
}

void Emulator::AllocateMemory(int size)
{
    memory = new uint8_t[size];
    for (int i = 0; i < size; i++)
        memory[i] = 0;
    mem_size = size;
}

// Copy contents of file specified by file_path into memory
// Returns number of bytes read
int Emulator::LoadRom(string file_path)
{
    streampos size;

    ifstream file(file_path, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        AllocateMemory(size);

        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char *>(memory), size);
        file.close();

        return size;
    }
    else
    {
        cout << "Unable to open file " << file_path << endl;
        return 0;
    }
}

// Determines parity flag
bool Emulator::parity(int x, int size = 8)
{
    int p = 0;
    x = (x & ((1 << size) - 1));
    for (int i = 0; i < size; i++)
    {
        if (x & 0x1)
        {
            p++;
        }
        x = x >> 1;
    }
    return (0 == (p & 0x1));
}

void Emulator::LogicFlagsA()
{
    flags.cy = (flags.ac = 0);
    flags.z = (registers.A == 0);
    flags.s = (0x80 == (registers.A & 0x80));
    flags.p = parity(registers.A);
}

void Emulator::ArithFlagsA(uint16_t res)
{
    flags.cy = (res > 0xff);
    flags.z = ((res & 0xff) == 0);
    flags.s = (0x80 == (res & 0x80));
    flags.p = parity(res & 0xff);
}

void Emulator::ZSPFlags(uint8_t value)
{
    flags.z = (value == 0);
    flags.s = (0x80 == (value & 0x80));
    flags.p = parity(value);
}

void Emulator::InvalidInstruction(uint8_t byte, uint16_t addr)
{
    cout << "Invalid instruction:" << endl;
    cout << "opcode 0x" << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(byte) << endl;
    cout << "at memory location 0x" << hex << setfill('0') << setw(4)
         << static_cast<unsigned>(addr) << endl;
    pc++;
}

void Emulator::WriteToMem(uint16_t address, uint8_t value)
{
    if (address < 0x2000 || address >= 0x4000)
    {
        cout << "Invalid write location " << address << endl;
        return;
    }

    memory[address] = value;
}

uint8_t Emulator::ReadFromMem(uint16_t address)
{
    return memory[address];
}

void Emulator::WriteToHL(uint8_t value)
{
    uint16_t offset = (registers.H << 8) | registers.L;
    WriteToMem(offset, value);
}

uint8_t Emulator::ReadFromHL()
{
    uint16_t offset = (registers.H << 8) | registers.L;
    return ReadFromMem(offset);
}

void Emulator::Call(uint8_t addr_high, uint8_t addr_low)
{
    uint16_t ret = pc + 3;
    Push((ret >> 8) & 0xff, (ret & 0xff));
    pc = (addr_high << 8) | addr_low;
}

void Emulator::Return()
{
    uint8_t addr_high;
    uint8_t addr_low;
    Pop(&addr_high, &addr_low);
    pc = (addr_high << 8) | addr_low;
}

void Emulator::Push(uint8_t high, uint8_t low)
{
    WriteToMem(sp - 1, high);
    WriteToMem(sp - 2, low);
    sp -= 2;
}

void Emulator::Pop(uint8_t *high, uint8_t *low)
{
    *low = memory[sp];
    *high = memory[sp + 1];
    sp += 2;
}

void Emulator::SubtractFromA(uint8_t operand)
{
    uint16_t num1 = registers.A;
    uint16_t num2 = ~operand & 0x00ff;
    uint16_t result = num1 + num2 + 0x0001;
    registers.A = result & 0x00ff;

    // Set flags
    flags.z = (registers.A == 0);
    flags.s = (registers.A & 0x80);
    flags.p = parity(registers.A);
    flags.cy = !(result & 0x0100);
}

void Emulator::Emulate()
{
    int count = 0;
    pc = 0;

    while (count < 110)
    {
        uint8_t opcode = memory[pc];

        Disassembler::Disassemble(reinterpret_cast<char *>(memory), pc);
        EmulateOpcode(opcode, memory[pc + 1], memory[pc + 2]);

        count++;
    }
}

void Emulator::EmulateOpcode(uint8_t opcode, uint8_t operand1, uint8_t operand2)
{
    switch (opcode)
    {
    // 0x00 - 0x0f
    case 0x00:
        // NOP
        {
            pc++;
        }
        break;
    case 0x01:
        // LXI B,D16
        {
            registers.B = operand2;
            registers.C = operand1;
            pc += 3;
        }
        break;
    case 0x02:
        // STAX B
        {
            uint16_t offset = (registers.B << 8) | registers.C;
            WriteToMem(offset, registers.A);
            pc += 1;
        }
        break;
    case 0x03:
        // INX B
        {
            registers.C++;
            if (registers.C == 0)
            {
                registers.B++;
            }
            pc++;
        }
        break;

    case 0x04:
        // INR B
        {
            registers.B++;
            ZSPFlags(registers.B);
            pc++;
        }
        break;

    case 0x05:
        // DCR B
        {
            registers.B--;
            ZSPFlags(registers.B);
            pc++;
        }
        break;

    case 0x06:
        // MVI B, D8
        {
            registers.B = operand1;
            pc += 2;
        }
        break;

    case 0x07:
        // RLC
        {
            flags.cy = (0x80 == (0x80 & registers.A));
            registers.A = registers.A << 1;
            if (flags.cy == 1)
            {
                registers.A++;
            }
            pc++;
        }
        break;

    case 0x08:
        // NOP
        {
            InvalidInstruction(opcode, pc);
        }
        break;

    case 0x09:
        // DAD B
        {
            uint32_t bc = (registers.B << 8) | registers.C;
            uint32_t hl = (registers.H << 8) | registers.L;
            uint32_t ans = bc + hl;
            registers.H = (ans & 0xff00) >> 8;
            registers.L = (ans & 0xff);
            flags.cy = ((ans & 0xffff0000) != 0);
            pc++;
        }
        break;

    case 0x0a:
        // LDAX B
        {
            uint16_t offset = (registers.B << 8) | registers.C;
            registers.A = memory[offset];
            pc++;
        }
        break;

    case 0x0b:
        // DCX B
        {
            uint16_t BC = ((uint16_t)registers.B << 8) | registers.C;
            BC--;
            registers.B = (uint8_t)(BC >> 8);
            registers.C = (uint8_t)BC;
            pc++;
        }
        break;

    case 0x0c:
        // INR C
        {
            registers.C++;
            ZSPFlags(registers.C);
            pc++;
        }
        break;

    case 0x0d:
        // DCR C
        {
            registers.C--;
            ZSPFlags(registers.C);
            pc++;
        }
        break;

    case 0x0e:
        // MVI C, D8
        {
            registers.C = operand1;
            pc += 2;
        }
        break;

    case 0x0f:
        // RRC
        {
            flags.cy = (0x01 == (registers.A & 0x01));
            registers.A = registers.A >> 1;
            if (flags.cy == 1)
            {
                registers.A = (registers.A | 0x80);
            }
            pc++;
        }
        break;

    // 0x10 - 0x1f
    case 0x10:
        // no instruction
        {
            InvalidInstruction(opcode, pc);
        }
        break;
    case 0x11:
        // LXI D, word
        // Load next two bytes into DE register pair
        {
            registers.D = operand2;
            registers.E = operand1;
            pc += 3;
        }
        break;
    case 0x12:
        // STAX D
        // Store the value in register A at the memory address stored in the DE register pair
        {
            uint16_t mem_addr = (registers.D << 8) | registers.E;
            WriteToMem(mem_addr, registers.A);
            pc++;
        }
        break;
    case 0x13:
        // INX D
        // Increment registers D and E, no flags affected
        {
            registers.E++;
            if (registers.E == 0x00)
            {
                registers.D++;
            }
            pc++;
        }
        break;
    case 0x14:
        // INR D
        {
            registers.D++;
            ZSPFlags(registers.D);

            pc++;
        }
        break;
    case 0x15:
        // DCR D
        // Decrement D
        {
            registers.D--;
            ZSPFlags(registers.D);

            pc++;
        }
        break;
    case 0x16:
        // MVI D, byte
        // Load next byte into register D
        {
            registers.D = operand1;
            pc += 2;
        }
        break;
    case 0x17:
        // RAL
        // Shift bits of A to the left, through carry (bit 0 = cy, cy = bit 7)
        {
            uint16_t temp = registers.A << 1;
            if (flags.cy == 1)
            {
                temp = temp | 0x0001;
            }
            flags.cy = (registers.A & 0x0080);
            registers.A = temp & 0x00FF;

            pc++;
        }
        break;
    case 0x18:
        // no instruction
        {
            InvalidInstruction(opcode, pc);
        }
        break;
    case 0x19:
        // DAD D
        // 16 bit addition DE + HL is stored in HL, cy set accordingly
        {
            uint32_t num1 = (registers.D << 8) | registers.E;
            uint32_t num2 = (registers.H << 8) | registers.L;
            uint32_t sum = num1 + num2;
            registers.H = (sum >> 8) & 0x000000FF;
            registers.L = sum & 0x000000FF;
            flags.cy = (sum & 0x00010000);

            pc++;
        }
        break;
    case 0x1a:
        // LDAX D
        // Load register A with byte at the memory address stored in the DE register pair
        {
            uint16_t mem_addr = (registers.D << 8) | registers.E;
            registers.A = memory[mem_addr];

            pc++;
        }
        break;
    case 0x1b:
        // DCX D
        // Decrement registers D and E as a 16 bit number, no flags affected
        {
            uint16_t DE = ((uint16_t)registers.D << 8) | registers.E;
            DE--;
            registers.D = (uint8_t)(DE >> 8);
            registers.E = (uint8_t)DE;
            pc++;
        }
        break;
    case 0x1c:
        // INR E
        {
            registers.E++;
            ZSPFlags(registers.E);
            pc++;
        }
        break;
    case 0x1d:
        // DCR E
        // Decrement register E and
        {
            registers.E--;
            ZSPFlags(registers.E);
            pc++;
        }
        break;
    case 0x1e:
        // MVI E, byte
        // Load next byte into register E
        {
            registers.E = operand1;
            pc += 2;
        }
        break;
    case 0x1f:
        // RAR
        // Shift bits of A to the right, through carry (bit 7 = cy, cy = bit 0)
        {
            uint16_t temp = registers.A >> 1;
            if (flags.cy == 1)
            {
                temp = temp | 0x0080;
            }
            flags.cy = (registers.A & 0x0001);
            registers.A = temp & 0x00FF;

            pc++;
        }
        break;

    // 0x20 - 0x2f
    case 0x20:
        /// NOP
        InvalidInstruction(opcode, pc);
        break;
    case 0x21:
        // LXI H, #$
        {
            registers.H = operand2;
            registers.L = operand1;
            pc += 3;
        }
        break;
    case 0x22:
        // SHLD $
        {
            uint16_t address = (operand2 << 8) | operand1;
            memory[address] = registers.L;
            memory[address + 1] = registers.H;
            pc += 3;
        }
        break;
    case 0x23:
        // INX H
        {
            registers.L++;
            // Carry if overflows
            if (registers.L == 0)
            {
                registers.H++;
            }
            pc++;
        }
        break;
    case 0x24:
        // INR H
        {
            registers.H++;
            ZSPFlags(registers.H);
            pc++;
        }
        break;
    case 0x25:
        // DCR H
        {
            registers.H--;
            ZSPFlags(registers.H);
            pc++;
        }
        break;
    case 0x26:
        // MVI H, #$
        {
            registers.H = operand1;
            pc += 2;
        }
        break;
    case 0x27:
        // DAA
        {
            uint8_t lowNibble = registers.A & 0x0F;
            uint8_t highNibble = registers.A >> 4;

            if (lowNibble > 9 || flags.ac)
            {
                registers.A += 6;
                flags.ac = 1;
            }

            if (highNibble > 9 || flags.cy)
            {
                registers.A += 0x60; // Increment most significant bits by 6
            }

            LogicFlagsA();
            pc++;
        }
        break;
    case 0x28:
        InvalidInstruction(opcode, pc);
        break;
    case 0x29:
        // DAD H
        {
            // Combine H and L
            uint32_t HL = (registers.H << 8) | registers.L;
            // Double HL
            HL <<= 1;
            // Set carry flag if necessary
            flags.cy = ((HL & 0xffff0000) != 0);
            // Save to registers
            registers.H = (HL & 0xff00) >> 8;
            registers.L = (HL & 0xff);
            pc++;
        }
        break;
    case 0x2a:
        // LHLD $
        {
            uint16_t address = (operand2 << 8) | operand1;
            registers.L = memory[address];
            registers.H = memory[address + 1];
            pc += 3;
        }
        break;
    case 0x2b:
        // DCX H
        {
            uint16_t HL = ((uint16_t)registers.H << 8) | registers.L;
            HL--;
            registers.H = (uint8_t)(HL >> 8);
            registers.L = (uint8_t)HL;
            pc++;
        }
        break;
    case 0x2c:
        // INR L
        {
            registers.L++;
            ZSPFlags(registers.L);
            pc++;
        }
        break;
    case 0x2d:
        // DCR L
        {
            registers.L--;
            ZSPFlags(registers.L);
            pc++;
        }
        break;
    case 0x2e:
        // MVI L, #$
        {
            registers.L = operand1;
            pc += 2;
        }
        break;
    case 0x2f:
        // CMA
        {
            // Bitwise NOT to get the complement of A
            registers.A = ~registers.A;
            pc++;
        }
        break;

    // 0x30 - 0x3f
    case 0x30:
        InvalidInstruction(opcode, pc);
        break;
    case 0x31:
        // LXI SP,word
        {
            sp = (operand2 << 8) | operand1;
            pc += 3;
        }
        break;
    case 0x32:
        // STA (word)
        {
            uint16_t offset = (operand2 << 8) | operand1;
            WriteToMem(offset, registers.A);
            pc += 3;
        }
        break;
    case 0x33:
        // INX SP
        {
            sp++;
            pc++;
        }
        break;
    case 0x34:
        // INR M
        {
            uint8_t res = ReadFromHL() + 1;
            ZSPFlags(res);
            WriteToHL(res);
            pc++;
        }
        break;
    case 0x35:
        // DCR M
        {
            uint8_t res = ReadFromHL() - 1;
            ZSPFlags(res);
            WriteToHL(res);
            pc++;
        }
        break;
    case 0x36:
        // MVI M, byte
        {
            WriteToHL(operand1);
            pc += 2;
        }
        break;
    case 0x37:
        // STC
        {
            flags.cy = 1;
            pc++;
        }
        break;
    case 0x38:
        InvalidInstruction(opcode, pc);
        break;
    case 0x39:
        // DAD SP
        {
            uint32_t hl = (registers.H << 8) | registers.L;
            uint32_t res = hl + sp;
            registers.H = (res & 0xff00) >> 8;
            registers.L = res & 0xff;
            flags.cy = ((res & 0xffff0000) > 0);
            pc++;
        }
        break;
    case 0x3a:
        // LDA (word)
        {
            uint16_t offset = (operand2 << 8) | operand1;
            registers.A = memory[offset];
            pc += 3;
        }
        break;
    case 0x3b:
        // DCX SP
        {
            sp -= 1;
            pc++;
        }
        break;
    case 0x3c:
        // INR A
        {
            registers.A++;
            ZSPFlags(registers.A);
            pc++;
        }
        break;
    case 0x3d:
        // DCR A
        {
            registers.A--;
            ZSPFlags(registers.A);
            pc++;
        }
        break;
    case 0x3e:
        // MVI A, byte
        {
            registers.A = operand1;
            pc += 2;
        }
        break;
    case 0x3f:
        // CMC
        {
            flags.cy = !flags.cy;
            pc++;
        }
        break;

    // 0x40 - 0x4f
    case 0x40:
        // MOV B,B
        {
            pc++;
        }
        break;

    case 0x41:
        // MOV B,C
        {
            registers.B = registers.C;
            pc++;
        }
        break;

    case 0x42:
        // MOV B,D
        {
            registers.B = registers.D;
            pc++;
        }
        break;

    case 0x43:
        // MOV B,E
        {
            registers.B = registers.E;
            pc++;
        }
        break;

    case 0x44:
        // MOV B,H
        {
            registers.B = registers.H;
            pc++;
        }
        break;

    case 0x45:
        // MOV B,L
        {
            registers.B = registers.L;
            pc++;
        }
        break;

    case 0x46:
        // MOV B,M
        {
            registers.B = ReadFromHL();
            pc++;
        }
        break;

    case 0x47:
        // MOV B,A
        {
            registers.B = registers.A;
            pc++;
        }
        break;

    case 0x48:
        // MOV C,B
        {
            registers.C = registers.B;
            pc++;
        }
        break;

    case 0x49:
        // MOV C,C
        {
            pc++;
        }
        break;

    case 0x4a:
        // MOV C,D
        {
            registers.C = registers.D;
            pc++;
        }
        break;

    case 0x4b:
        // MOV C,E
        {
            registers.C = registers.E;
            pc++;
        }
        break;

    case 0x4c:
        // MOV C,H
        {
            registers.C = registers.H;
            pc++;
        }
        break;

    case 0x4d:
        // MOV C,L
        {
            registers.C = registers.L;
            pc++;
        }
        break;

    case 0x4e:
        // MOV C,M
        {
            registers.C = ReadFromHL();
            pc++;
        }
        break;

    case 0x4f:
        // MOV C,A
        {
            registers.C = registers.A;

            flags.cy = 0;
            pc++;
        }
        break;

    // 0x50 - 0x5f
    case 0x50:
        // MOV D, B
        {
            registers.D = registers.B;
            pc++;
        }
        break;
    case 0x51:
        // MOV D, C
        {
            registers.D = registers.C;
            pc++;
        }
        break;
    case 0x52:
        // MOV D, D
        {
            pc++;
        }
        break;
    case 0x53:
        // MOV D, E
        {
            registers.D = registers.E;
            pc++;
        }
        break;
    case 0x54:
        // MOV D, H
        {
            registers.D = registers.H;
            pc++;
        }
        break;
    case 0x55:
        // MOV D, L
        {
            registers.D = registers.L;
            pc++;
        }
        break;
    case 0x56:
        // MOV D, M
        {
            uint16_t mem_addr = (registers.H << 8) | (registers.L);
            registers.D = memory[mem_addr];
            pc++;
        }
        break;
    case 0x57:
        // MOV D, A
        {
            registers.D = registers.A;
            pc++;
        }
        break;
    case 0x58:
        // MOV E, B
        {
            registers.E = registers.B;
            pc++;
        }
        break;
    case 0x59:
        // MOV E, C
        {
            registers.E = registers.C;
            pc++;
        }
        break;
    case 0x5a:
        // MOV E, D
        {
            registers.E = registers.D;
            pc++;
        }
        break;
    case 0x5b:
        // MOV E, E
        {
            pc++;
        }
        break;
    case 0x5c:
        // MOV E, H
        {
            registers.E = registers.H;
            pc++;
        }
        break;
    case 0x5d:
        // MOV E, L
        {
            registers.E = registers.L;
            pc++;
        }
        break;
    case 0x5e:
        // MOV E, M
        {
            registers.E = ReadFromHL();
            pc++;
        }
        break;
    case 0x5f:
        // MOV E, A
        {
            registers.E = registers.A;
            pc++;
        }
        break;

    // 0x60 - 0x6f
    case 0x60:
        // MOV H, B
        {
            registers.H = registers.B;
            pc++;
        }
        break;
    case 0x61:
        // MOV H, C
        {
            registers.H = registers.C;
            pc++;
        }
        break;
    case 0x62:
        // MOV H, D
        {
            registers.H = registers.D;
            pc++;
        }
        break;
    case 0x63:
        // MOV H, E
        {
            registers.H = registers.E;
            pc++;
        }
        break;
    case 0x64:
        // MOV H, H
        {
            pc++;
        }
        break;
    case 0x65:
        // "MOV H, L
        {
            registers.H = registers.L;
            pc++;
        }
        break;
    case 0x66:
        // "MOV H, M
        {
            registers.H = ReadFromHL();
            pc++;
        }
        break;
    case 0x67:
        // "MOV H, A
        {
            registers.H = registers.A;
            pc++;
        }
        break;
    case 0x68:
        // "MOV L, B
        {
            registers.L = registers.B;
            pc++;
        }
        break;
    case 0x69:
        // "MOV L, C
        {
            registers.L = registers.C;
            pc++;
        }
        break;
    case 0x6a:
        // "MOV L, D
        {
            registers.L = registers.D;
            pc++;
        }
        break;
    case 0x6b:
        // "MOV L, E
        {
            registers.L = registers.E;
            pc++;
        }
        break;
    case 0x6c:
        // "MOV L, H
        {
            registers.L = registers.H;
            pc++;
        }
        break;
    case 0x6d:
        // "MOV L, L
        {
            pc++;
        }
        break;
    case 0x6e:
        // "MOV L, M
        {
            registers.L = ReadFromHL();
            pc++;
        }
        break;
    case 0x6f:
        // "MOV L, A
        {
            registers.L = registers.A;
            pc++;
        }
        break;

    // 0x70 - 0x7f
    case 0x70:
        // MOV M, B
        {
            WriteToHL(registers.B);
            pc++;
        }
        break;
    case 0x71:
        // MOV M, C
        {
            WriteToHL(registers.C);
            pc++;
        }
        break;
    case 0x72:
        // MOV M, D
        {
            WriteToHL(registers.D);
            pc++;
        }
        break;
    case 0x73:
        // MOV M, E
        {
            WriteToHL(registers.E);
            pc++;
        }
        break;
    case 0x74:
        // MOV M, H
        {
            WriteToHL(registers.H);
            pc++;
        }
        break;
    case 0x75:
        // MOV M, L
        {
            WriteToHL(registers.L);
            pc++;
        }
        break;
    case 0x76:
        // HLT
        {
            pc++;
        }
        break;
    case 0x77:
        // MOV M, A
        {
            WriteToHL(registers.A);
            pc++;
        }
        break;
    case 0x78:
        // MOV A, B
        {
            registers.A = registers.B;
            pc++;
        }
        break;
    case 0x79:
        // MOV A, C
        {
            registers.A = registers.C;
            pc++;
        }
        break;
    case 0x7a:
        // MOV A, D
        {
            registers.A = registers.D;
            pc++;
        }
        break;
    case 0x7b:
        // MOV A, E
        {
            registers.A = registers.E;
            pc++;
        }
        break;
    case 0x7c:
        // MOV A, H
        {
            registers.A = registers.H;
            pc++;
        }
        break;
    case 0x7d:
        // MOV A, L
        {
            registers.A = registers.L;
            pc++;
        }
        break;
    case 0x7e:
        // MOV A, HL
        {
            registers.A = ReadFromHL();
            pc++;
        }
        break;
    case 0x7f:
        // MOV A, A
        {
            registers.A = registers.A;
            pc++;
        }
        break;

    // 0x80 - 0x8f
    case 0x80:
        // ADD B
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.B;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x81:
        // ADD C
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.C;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x82:
        // ADD D
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.D;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x83:
        // ADD E
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.E;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x84:
        // ADD H
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.H;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x85:
        // ADD L
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.L;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x86:
        // ADD M
        {
            uint16_t res = (uint16_t)registers.A + ReadFromHL();
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x87:
        // ADD A
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)registers.A;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x88:
        // ADC B
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.B + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x89:
        // ADC C
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.C + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x8a:
        // ADC D
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.D + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x8b:
        // ADC E
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.E + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x8c:
        // ADC H
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.H + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x8d:
        // ADC L
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.L + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x8e:
        // ADC M
        {
            uint32_t res = (uint16_t)registers.A + ReadFromHL() + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    case 0x8f:
        // ADC A
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)registers.A + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc++;
        }
        break;

    // 0x90 - 0x9f
    case 0x90:
        // SUB B
        // Subtract register B from register A and store result in A
        {
            SubtractFromA(registers.B);
            pc++;
        }
        break;
    case 0x91:
        // SUB C
        // Subtract register C from register A and store result in A
        {
            SubtractFromA(registers.C);
            pc++;
        }
        break;
    case 0x92:
        // SUB D
        // Subtract register D from register A and store result in A
        {
            SubtractFromA(registers.D);
            pc++;
        }
        break;
    case 0x93:
        // SUB E
        // Subtract register E from register A and store result in A
        {
            SubtractFromA(registers.E);
            pc++;
        }
        break;
    case 0x94:
        // SUB H
        // Subtract register H from register A and store result in A
        {
            SubtractFromA(registers.H);
            pc++;
        }
        break;
    case 0x95:
        // SUB L
        // Subtract register L from register A and store result in A
        {
            SubtractFromA(registers.L);
            pc++;
        }
        break;
    case 0x96:
        // SUB M
        // Subtract byte from memory at address stored in HL from register A and store result in A
        {
            uint8_t operand = ReadFromHL();
            SubtractFromA(operand);
            pc++;
        }
        break;
    case 0x97:
        // SUB A
        // Subtract register A from register A and store result in A
        {
            SubtractFromA(registers.A);
            pc++;
        }
        break;
    case 0x98:
        // SBB B
        // Subtract register B (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.B + flags.cy);
            pc++;
        }
        break;
    case 0x99:
        // SBB C
        // Subtract register C (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.C + flags.cy);
            pc++;
        }
        break;
    case 0x9a:
        // SBB D
        // Subtract register D (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.D + flags.cy);
            pc++;
        }
        break;
    case 0x9b:
        // SBB E
        // Subtract register E (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.E + flags.cy);
            pc++;
        }
        break;
    case 0x9c:
        // SBB H
        // Subtract register H (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.H + flags.cy);
            pc++;
        }
        break;
    case 0x9d:
        // SBB L
        // Subtract register L (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.L + flags.cy);
            pc++;
        }
        break;
    case 0x9e:
        // SBB M
        // Subtract byte in memory (location in HL) from register A and store result in A
        {
            uint8_t operand = ReadFromHL();
            SubtractFromA(operand + flags.cy);
            pc++;
        }
        break;
    case 0x9f:
        // SBB A
        // Subtract register A (plus carry) from register A and store result in A
        {
            SubtractFromA(registers.A + flags.cy);
            pc++;
        }
        break;

    // 0xa0 - 0xaf
    case 0xa0:
        // ANA B
        {
            registers.A &= registers.B;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa1:
        // ANA C
        {
            registers.A &= registers.C;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa2:
        // ANA D
        {
            registers.A &= registers.D;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa3:
        // ANA E
        {
            registers.A &= registers.E;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa4:
        // ANA H
        {
            registers.A &= registers.H;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa5:
        // ANA L
        {
            registers.A &= registers.L;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa6:
        // ANA M
        {
            registers.A &= ReadFromHL();
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa7:
        // ANA A
        {
            registers.A &= registers.A;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa8:
        // XRA B
        {
            registers.A ^= registers.B;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xa9:
        // XRA C
        {
            registers.A ^= registers.C;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xaa:
        // XRA D
        {
            registers.A ^= registers.D;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xab:
        // XRA E
        {
            registers.A ^= registers.E;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xac:
        // XRA H
        {
            registers.A ^= registers.H;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xad:
        // XRA L
        {
            registers.A ^= registers.L;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xae:
        // XRA M
        {
            registers.A ^= ReadFromHL();
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xaf:
        // XRA A
        {
            registers.A ^= registers.A;
            LogicFlagsA();
            pc++;
        }
        break;

    // 0xb0 - 0xbf
    case 0xb0:
        // ORA B
        {
            registers.A = registers.A | registers.B;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb1:
        // ORA C
        {
            registers.A = registers.A | registers.C;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb2:
        // ORA D
        {
            registers.A = registers.A | registers.D;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb3:
        // ORA E
        {
            registers.A = registers.A | registers.E;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb4:
        // ORA H
        {
            registers.A = registers.A | registers.H;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb5:
        // ORA L
        {
            registers.A = registers.A | registers.L;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb6:
        // ORA M
        {
            registers.A = registers.A | ReadFromHL();
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb7:
        // ORA A
        {
            registers.A = registers.A | registers.A;
            LogicFlagsA();
            pc++;
        }
        break;
    case 0xb8:
        // CMP B
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.B;
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xb9:
        // CMP C
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.C;
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xba:
        // CMP D
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.D;
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xbb:
        // CMP E
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.E;
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xbc:
        // CMP H
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.H;
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xbd:
        // CMP L
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.L;
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xbe:
        // CMP HL
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)ReadFromHL();
            ArithFlagsA(res);
            pc++;
        }
        break;
    case 0xbf:
        // CMP A
        {
            uint16_t res = (uint16_t)registers.A - (uint16_t)registers.A;
            ArithFlagsA(res);
            pc++;
        }
        break;

    // 0xc0 - 0xcf
    case 0xc0:
        // RNZ
        {
            if (flags.z == 0)
            {
                Return();
            }
            else
            {
                pc++;
            }
        }
        break;

    case 0xc1:
        // POP B
        {
            Pop(&registers.B, &registers.C);
            pc++;
        }
        break;

    case 0xc2:
        // JNZ adr
        {
            if (flags.z == 0)
            {
                pc = (operand2 << 8) | operand1;
            }
            else
            {
                pc += 3;
            }
        }
        break;

    case 0xc3:
        // JMP
        {
            pc = (operand2 << 8) | operand1;
        }
        break;

    case 0xc4:
        // CNZ
        {
            if (flags.z == 0)
            {
                Call(operand2, operand1);
            }
            else
            {
                pc += 3;
            }
        }
        break;

    case 0xc5:
        // PUSH B
        {
            Push(registers.B, registers.C);
            pc++;
        }
        break;

    case 0xc6:
        // ADI D8
        {
            uint16_t res = (uint16_t)registers.A + (uint16_t)operand1;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc += 2;
        }
        break;

    case 0xc7:
        // RST 0
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0000;
        }
        break;

    case 0xc8:
        // RZ
        {
            if (flags.z == 1)
            {
                Return();
            }
            else
            {
                pc++;
            }
        }
        break;

    case 0xc9:
        // RET
        {
            Return();
        }
        break;

    case 0xca:
        // JZ
        {
            if (flags.z == 1)
            {
                pc = (operand2 << 8) | operand1;
            }
            else
            {
                pc += 3;
            }
        }
        break;

    case 0xcb:
        // NOP
        {
            InvalidInstruction(opcode, pc);
        }
        break;

    case 0xcc:
        // CZ
        {
            if (flags.z == 1)
            {
                Call(operand2, operand1);
            }
            else
            {
                pc += 3;
            }
        }
        break;

    case 0xcd:
        // CALL
        {
            Call(operand2, operand1);
        }
        break;

    case 0xce:
        // ACI D8
        {
            uint16_t res = (uint16_t)registers.A +
                           (uint16_t)operand1 + flags.cy;
            ArithFlagsA(res);
            registers.A = (uint8_t)res;
            pc += 2;
        }
        break;

    case 0xcf:
        // RST 1
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0008;
        }
        break;

    // 0xd0 - 0xdf
    case 0xd0:
        // RNC
        // Return if no carry
        {
            if (!flags.cy)
            {
                Return();
            }
            else
            {
                pc++;
            }
        }
        break;
    case 0xd1:
        // POP D
        // Pop top two bytes of stack to registers D and E
        {
            Pop(&registers.D, &registers.E);
            pc++;
        }
        break;
    case 0xd2:
        // JNC
        // Jump if no carry
        {
            if (!flags.cy)
            {
                uint8_t addr_high = operand2;
                uint8_t addr_low = operand1;
                pc = (addr_high << 8) | addr_low;
            }
            else
            {
                pc += 3;
            }
        }
        break;
    case 0xd3:
        // OUT
        // Send contents of register A to output device determined by next byte
        {
            // special
            pc++;
        }
        break;
    case 0xd4:
        // CNC
        // Call if no carry
        {
            if (!flags.cy)
            {
                Call(operand2, operand1);
            }
            else
            {
                pc += 3;
            }
        }
        break;
    case 0xd5:
        // PUSH D
        // Push registers D and E to the stack
        {
            Push(registers.D, registers.E);
            pc++;
        }
        break;
    case 0xd6:
        // SUI
        // Subtract immediate from A
        {
            uint8_t operand = operand1;
            SubtractFromA(operand);
            pc += 2;
        }
        break;
    case 0xd7:
        // RST 2
        // calls program at address 0x0010
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0010;
        }
        break;
    case 0xd8:
        // RC
        // Return if carry
        {
            if (flags.cy)
            {
                Return();
            }
            else
            {
                pc++;
            }
        }
        break;
    case 0xd9:
        // NOP
        {
            InvalidInstruction(opcode, pc);
        }
        break;
    case 0xda:
        // JC
        // Jump if carry
        {
            if (flags.cy)
            {
                uint16_t addr_high = operand2;
                uint16_t addr_low = operand1;
                pc = (addr_high << 8) | addr_low;
            }
            else
            {
                pc += 3;
            }
        }
        break;
    case 0xdb:
        // IN
        // One byte of input is read from the input device specified by next byte
        // and stored in register A
        {
            // special
            pc++;
        }
        break;
    case 0xdc:
        // CC
        // Call if carry
        {
            if (flags.cy)
            {
                Call(operand2, operand1);
            }
            else
            {
                pc += 3;
            }
        }
        break;
    case 0xdd:
        // NOP
        {
            InvalidInstruction(opcode, pc);
        }
        break;
    case 0xde:
        // SBI
        // Subtract immediate from accumulator with borrow
        {
            SubtractFromA(operand1 + flags.cy);
            pc += 2;
        }
        break;
    case 0xdf:
        // RST 3
        // Calls program at address 0x0018
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0018;
        }
        break;

    // 0xe0 - 0xef
    case 0xe0:
        // RPO - Return if parity odd
        if (flags.p == 0)
        {
            Return();
        }
        else
        {
            pc++;
        }
        break;
    case 0xe1:
        // POP H
        {
            registers.L = memory[sp];
            registers.H = memory[sp + 1];
            sp += 2;
            pc++;
        }
        break;
    case 0xe2:
        // JPO $
        {
            // Parity flag = 1 indicates even
            if (flags.p == 1)
            {
                pc += 3;
            }
            else
            {
                pc = (operand2 << 8) | operand1;
            }
        }
        break;
    case 0xe3:
        // XTHL
        {
            uint8_t tempL = registers.L;
            uint8_t tempH = registers.H;
            registers.L = memory[sp];
            registers.H = memory[sp + 1];
            memory[sp] = tempL;
            memory[sp + 1] = tempH;
            pc++;
        }
        break;
    case 0xe4:
        // CPO $
        {
            if (flags.p == 0)
            {
                Call(operand2, operand1);
            }
            else
            {
                pc += 3; // Skip over the address if parity is not odd
            }
        }
        break;
    case 0xe5:
        // PUSH H
        {
            sp -= 2;
            memory[sp + 1] = registers.H;
            memory[sp] = registers.L;
            pc++;
        }
        break;
    case 0xe6:
        // ANI #$
        {
            registers.A &= operand1;
            LogicFlagsA();
            pc += 2;
        }
        break;
    case 0xe7:
        // RST 4
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0020;
        }
        break;
    case 0xe8:
        // RPE - Return if parity even
        if (flags.p == 1)
        {
            Return();
        }
        else
        {
            pc++;
        }
        break;
    case 0xe9:
        // PCHL
        {
            pc = (registers.H << 8) | registers.L;
        }
        break;
    case 0xea:
        // JPE $
        {
            // Parity flag = 0 indicates odd
            if (flags.p == 0)
            {
                pc += 3;
            }
            else
            {
                pc = (operand2 << 8) | operand1;
            }
        }
        break;
    case 0xeb:
        // XCHG - Swap HL with DE
        {
            uint8_t tempH = registers.H;
            uint8_t tempL = registers.L;
            registers.H = registers.D;
            registers.L = registers.E;
            registers.D = tempH;
            registers.E = tempL;
        }
        break;
    case 0xec:
        // CPE $
        {
            if (flags.p == 1)
            {
                Call(operand2, operand1);
            }
            else
            {
                pc += 3; // Skip over the address if parity is not odd
            }
        }
        break;
    case 0xed:
        // No instruction
        {
            InvalidInstruction(opcode, pc);
        }
        break;
    case 0xee:
        // XRI #$
        {
            registers.A ^= operand1;
            LogicFlagsA();
            pc += 2;
        }
        break;
    case 0xef:
        // RST 5
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0028;
        }
        break;

    // 0xf0 - 0xff
    case 0xf0:
        // RP
        if (flags.s == 0)
        {
            Return();
        }
        else
        {
            pc++;
        }
        break;
    case 0xf1:
        // POP PSW
        {
            registers.A = memory[sp + 1];
            uint8_t psw = memory[sp];
            flags.z = (0x01 == (psw & 0x01));
            flags.s = (0x02 == (psw & 0x02));
            flags.p = (0x04 == (psw & 0x04));
            flags.cy = (0x08 == (psw & 0x08)); // (0x05 == (psw & 0x08)) in reference. Typo? Equates to always false
            flags.ac = (0x10 == (psw & 0x10));
            sp += 2;
            pc++;
        }
        break;
    case 0xf2:
        if (flags.s == 0)
        {
            pc = (operand2 << 8) | operand1;
        }
        else
        {
            pc += 3;
        }
        break;
    case 0xf3:
        // DI
        {
            // interupt_enable = 0;
            pc++;
        }
        break;
    case 0xf4:
        // CP
        if (flags.s == 0)
        {
            Call(operand2, operand1);
        }
        else
        {
            pc += 3;
        }
        break;
    case 0xf5:
        // PUSH PSW
        {
            memory[sp - 1] = registers.A;
            uint8_t psw = (flags.z | flags.s << 1 | flags.p << 2 | flags.cy << 3 | flags.ac << 4);
            memory[sp - 2] = psw;
            sp -= 2;
            pc++;
        }
        break;
    case 0xf6:
        // ORI byte
        {
            uint8_t x = registers.A | operand1;
            ZSPFlags(x);
            flags.cy = 0;
            registers.A = x;
            pc++;
        }
        break;
    case 0xf7:
        // RST 6
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0030;
        }
        break;
    case 0xf8:
        // RM
        if (flags.s != 0)
        {
            Return();
        }
        else
        {
            pc++;
        }
        break;
    case 0xf9:
        // SPHL
        {
            sp = registers.L | (registers.H << 8);
            pc++;
        }
        break;
    case 0xfa:
        // JM
        if (flags.s != 0)
        {
            pc = (operand2 << 8) | operand1;
        }
        else
        {
            pc += 3;
        }
        break;
    case 0xfb:
        // EI
        {
            // interupt_enable = 1; Interupt enable variable not established in Emulator class yet
            pc++;
        }
        break;
    case 0xfc:
        // CM
        if (flags.s != 0)
        {
            Call(operand2, operand1);
        }
        else
        {
            pc += 3;
        }
        break;
    case 0xfd:
        // no instruction
        {
            InvalidInstruction(opcode, pc);
        }
        break;
    case 0xfe:
        // CPI byte
        {
            uint8_t mem = registers.A - operand1;
            ZSPFlags(mem);
            flags.cy = registers.A < operand1;
            pc++;
        }
    case 0xff:
        // RST 7
        {
            uint16_t ret_addr = pc + 1;
            uint8_t ret_high = (ret_addr >> 8) & 0x00ff;
            uint8_t ret_low = ret_addr & 0x00ff;
            Push(ret_high, ret_low);
            pc = 0x0038;
        }
        break;
    default:
        // unknown instruction
        {
            pc++;
        }
        break;
    }
}

// Print contents of all registers
void Emulator::PrintRegisters()
{
    cout << "Register A: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.A) << endl;
    cout << "Register B: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.B) << endl;
    cout << "Register C: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.C) << endl;
    cout << "Register D: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.D) << endl;
    cout << "Register E: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.E) << endl;
    cout << "Register H: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.H) << endl;
    cout << "Register L: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>(registers.L) << endl;
}

// Print current state of all condition codes
void Emulator::PrintFlags()
{
    cout << "Zero Flag:      " << flags.z << endl;
    cout << "Sign Flag:      " << flags.s << endl;
    cout << "Parity Flag:    " << flags.p << endl;
    cout << "Carry Flag:     " << flags.cy << endl;
    cout << "Aux Carry Flag: " << flags.ac << endl;
}

Registers Emulator::GetRegisters()
{
    return registers;
}

Flags Emulator::GetFlags()
{
    return flags;
}

int Emulator::GetPC()
{
    return pc;
}

int Emulator::GetSP()
{
    return sp;
}

void Emulator::SetSP(uint16_t new_sp)
{
    sp = new_sp;
}
