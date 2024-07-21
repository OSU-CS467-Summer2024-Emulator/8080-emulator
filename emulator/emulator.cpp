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

int Emulator::parity(int x, int size=8)
{
    int p = 0;
    x = (x & ((1<<size)-1));
    for (int i = 0; i<size; i++)
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
                pc +=2 ;
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
            }

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
