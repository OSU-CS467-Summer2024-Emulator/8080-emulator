#include <iostream>
#include <iomanip>
#include <fstream>
#include "./emulator.hpp"
#include "../disassembler/disassembler.hpp"

using namespace std;

// Constructor
Emulator::Emulator()
{
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

// Copy contents of file specified by file_path into memory
// Returns number of bytes read
int Emulator::LoadRom(string file_path)
{
    streampos size;
    char *mem_buffer;

    ifstream file(file_path, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memory = new unsigned char[size];

        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char *>(memory), size);
        file.close();

        return size;
    }
    else
    {
        cout << "Unable to open file" << file_path << endl;
        return 0;
    }
}

void Emulator::WriteToMem(uint16_t address, uint8_t value)
{
    if (address < 0x2000 || address >= 0x4000)
    {
        cout << "Invalid write location" << value << endl;
        return;
    }
    memory[address] = value;
}

void Emulator::SetArithFlags(uint8_t value)
{
    flags.cy = (value > 0xff);
    flags.z = ((0xff & value) == 0);
    flags.s = ((0x80 & value) == 0x80);
    flags.p = parity(value & 0xff);
}

void Emulator::SetLogicFlags(uint8_t value)
{
    flags.cy = ((0x80 & value) == 0x80);
}

void Emulator::FlagsZSP(uint8_t value)
{
    flags.z = (value == 0);
    flags.s = ((0x80 & value) == 0x80);
    flags.p = parity(value);
}

void Emulator::Pop(uint8_t *high, uint8_t *low)
{
    *low = memory[sp];
    *high = memory[sp + 1];
    sp += 2;
}

void Emulator::Push(uint8_t high, uint8_t low)
{
    WriteToMem(sp - 1, high);
    WriteToMem(sp - 2, low);
    sp -= 2;
}

int Emulator::parity(int x, int size = 8)
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

void Emulator::Emulate()
{
    int count = 0;
    pc = 0;

    while (count < 100)
    {
        unsigned char opcode = memory[pc];
        Disassemble((char *)memory, pc);

        switch (opcode)
        {
        // 0x00 - 0x0f
        case 0x00:
            // NOP
            pc++;
            break;
        case 0x01:
            // LXI B,D16
            {
                registers.B = memory[pc + 2];
                registers.C = memory[pc + 1];
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
                FlagsZSP(registers.B);
                pc++;
            }
            break;

        case 0x05:
            // DCR B
            {
                registers.B--;
                FlagsZSP(registers.B);
                pc++;
            }
            break;

        case 0x06:
            // MVI B, D8
            {
                registers.B = memory[pc + 1];
                pc += 2;
            }
            break;

        case 0x07:
            // RLC
            {
                flags.cy = (0x80 == (0x80 & registers.A));
                registers.A << 1;
                if (flags.cy == 1)
                {
                    registers.A++;
                }
                pc++;
            }
            break;

        case 0x08:
            // NOP
            pc++;
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
                registers.C--;
                if (registers.C == 0xff)
                {
                    registers.B--;
                }
                pc++;
            }
            break;

        case 0x0c:
            // INR C
            {
                registers.C++;
                FlagsZSP(registers.C);
                pc++;
            }
            break;

        case 0x0d:
            // DCR C
            {
                registers.C--;
                FlagsZSP(registers.C);
                pc++;
            }
            break;

        case 0x0e:
            // MVI C, D8
            {
                registers.C = memory[pc + 1];
                pc += 2;
            }
            break;

        case 0x0f:
            // RRC
            {
                flags.cy = (0x01 == (registers.A & 0x01));
                registers.A >> 1;
                if (flags.cy == 1)
                {
                    registers.A = (registers.A | 0x80);
                }
                pc++;
            }
            break;

        // 0x40 - 0x4f
        case 0x40:
            // MOV B,B
            pc++;
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
                uint16_t offset = (registers.H << 8) | registers.L;
                registers.B = memory[offset];
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
                uint16_t offset = (registers.H << 8) | registers.L;
                registers.C = memory[offset];
                pc++;
            }
            break;
        
        case 0x4f:
            // MOV C,A
            {
                registers.C = registers.A;
                pc++;
            }
            break;

        // 0x50 - 0x5f
        case 0x50:
            // MOV D, B
            {
                registers.D = registers.B;
            }
            break;
        case 0x51:
            // MOV D, C
            {
                registers.D = registers.C;
            }
            break;
        case 0x52:
            // MOV D, D
            break;
        case 0x53:
            // MOV D, E
            {
                registers.D = registers.E;
            }
            break;
        case 0x54:
            // MOV D, H
            {
                registers.D = registers.H;
            }
            break;
        case 0x55:
            // MOV D, L
            {
                registers.D = registers.L;
            }
            break;
        case 0x56:
            // MOV D, M
            {
                int mem_addr = (registers.H << 8) | (registers.L);
                registers.D = memory[mem_addr];
            }
            break;
        case 0x57:
            // MOV D, A
            {
                registers.D = registers.A;
            }
            break;

            // 0x50 - 0x5f
        case 0x58:
            // MOV E, B
            {
                registers.E = registers.B;
            }
            break;
        case 0x59:
            // MOV E, C
            {
                registers.E = registers.C;
            }
            break;
        case 0x5a:
            // MOV E, D
            {
                registers.E = registers.D;
            }
            break;
        case 0x5b:
            // MOV E, E
            break;
        case 0x5c:
            // MOV E, H
            {
                registers.E = registers.H;
            }
            break;
        case 0x5d:
            // MOV E, L
            {
                registers.E = registers.L;
            }
            break;
        case 0x5e:
            // MOV E, M
            {
                int mem_addr = (registers.H << 8) | (registers.L);
                registers.E = memory[mem_addr];
            }
            break;
        case 0x5f:
            // MOV E, A
            {
                registers.E = registers.A;
            }
            break;
        
        // 0x80 - 0x8f
        case 0x80:
            // ADD B
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.B;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x81:
            // ADD C
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.C;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x82:
            // ADD D
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.D;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x83:
            // ADD E
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.E;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x84:
            // ADD H
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.H;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x85:
            // ADD L
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.L;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x86:
            // ADD M
            {
                uint16_t offset = (registers.H << 8) | registers.L;
                uint32_t res = (uint16_t) registers.A + offset;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x87:
            // ADD A
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.A;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x88:
            // ADC B
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.B + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;

        case 0x89:
            // ADC C
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.C + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x8a:
            // ADC D
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.D + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x8b:
            // ADC E
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.E + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x8c:
            // ADC H
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.H + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;

        case 0x8d:
            // ADC L
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.L + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x8e:
            // ADC M
            {
                uint16_t offset = (registers.H << 8) | registers.L;
                uint32_t res = (uint16_t) registers.A + offset + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        case 0x8f:
            // ADC A
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) registers.A + flags.cy;
                SetArithFlags(res);
                registers.A = (res & 0xff);
                pc++;
            }
            break;
        
        // 0xc0 - 0xcf
        case 0xc0:
            // RNZ
            {
                if (flags.z == 0)
                {
                    pc = memory[sp] | (memory[sp + 1] << 8);
                    sp += 2;
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
                    pc = (memory[pc + 2] << 8) | memory[pc + 1];
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
                pc = (memory[pc + 2] << 8) | memory[pc + 1];
            }
            break;
        
        case 0xc4:
            // CNZ
            {
                if (flags.z == 0)
                {
                    uint16_t ret = pc + 3;
                    Push((ret >> 8) & 0xff, (ret & 0xff));
                    pc = (memory[pc + 2] << 8) | memory[pc + 1];
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
                uint16_t res = (uint16_t) registers.A + (uint16_t) memory[pc+ 1];
                SetArithFlags(res);
                registers.A = (uint8_t) res;
                pc++;
            }
            break;
        
        case 0xc7:
            // RST 0
            {
                // reference uses return address as pc+3, but I think it should
                // be pc + 1 since it is a 1 byte operation
                uint16_t ret = pc + 1;
                Push((ret >> 8) & 0xff, ret & 0xff);
                pc = 0x0000;
            }
            break;
        
        case 0xc8:
            // RZ
            {
                if (flags.z == 1)
                {
                    pc = memory[sp] | (memory[sp + 1] << 8);
                    sp += 2;
                }
            }
            break;
        
        case 0xc9:
            // RET
            {
                pc = memory[sp] | (memory[sp + 1] << 8);
                sp += 2;
            }
            break;
        
        case 0xca:
            // JZ
            {
                if (flags.z == 1)
                {
                    pc = (memory[pc + 2] << 8) | memory[pc + 1];
                }
                else
                {
                    pc += 3;
                }
            }
            break;
        
        case 0xcb:
            // NOP
            break;
        
        case 0xcc:
            // CZ
            {
                if (flags.z == 1)
                {
                    uint16_t ret = pc + 3;
                    Push((ret >> 8) & 0xff, (ret & 0xff));
                    pc = (memory[pc + 2] << 8) | memory[pc + 1];
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
                uint16_t ret = pc + 3;
                Push((ret >> 8) & 0xff, (ret & 0xff));
                pc = (memory[pc + 2] << 8) | memory[pc + 1];
            }
            break;
        
        case 0xce:
            // ACI D8
            {
                uint16_t res = (uint16_t) registers.A + (uint16_t) memory[pc+ 1] + flags.cy;
                SetArithFlags(res);
                registers.A = (uint8_t) res;
                pc++;
            }
            break;
 
        case 0xcf:
            // RST 1
            {
                // reference uses return address as pc+3, but I think it should
                // be pc + 1 since it is a 1 byte operation
                uint16_t ret = pc + 1;
                Push((ret >> 8) & 0xff, ret & 0xff);
                pc = 0x0008;
            }
            break;

        default:
            // unknown instruction
            break;
        }
        count += 1;
    }
}

// Print contents of all registers
void Emulator::PrintRegisters()
{
    cout << "Register A: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.A) << endl;
    cout << "Register B: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.B) << endl;
    cout << "Register C: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.C) << endl;
    cout << "Register D: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.D) << endl;
    cout << "Register E: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.E) << endl;
    cout << "Register H: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.H) << endl;
    cout << "Register L: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char)registers.L) << endl;
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
