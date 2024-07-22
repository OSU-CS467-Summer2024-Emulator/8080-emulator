#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include "./emulator.hpp"

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

void Emulator::Emulate()
{
    int count = 0;

    while (count < 100)
    {
        int opbytes = 1;
        unsigned char opcode = memory[pc];

        switch (opcode)
        {
        case 0x00:
            // NOP
            break;
        case 0x01:

            break;
        case 0x02:

            break;

        // 0x10 - 0x1f
        case 0x10:
            // no instruction
            {
                pc++;
            }
            break;
        case 0x11:
            // LXI D, word
            // Load next two bytes into DE register pair
            {
                registers.D = memory[pc + 2];
                registers.E = memory[pc + 1];
                pc += 3;
            }
            break;
        case 0x12:
            // STAX D
            // Store the value in register A at the memory address stored in the DE register pair
            {
                uint16_t mem_addr = (registers.D << 8) | registers.E;
                memory[mem_addr] = registers.A;
                pc++;
            }
            break;
        case 0x13:
            // INX D
            // Increment registers D and E, no flags affected
            {
                registers.D++;
                registers.E++;
                pc++;
            }
            break;
        case 0x14:
            // INR D
            // Increment D
            {
                registers.D++;
                // Set flags
                flags.z = (registers.D == 0);
                flags.s = (0x80 & registers.D);
                // TODO: implement parity function
                // flags.p = (parity(registers.D));

                pc++;
            }
            break;
        case 0x15:
            // DCR D
            // Decrement D
            {
                registers.D--;
                // Set flags
                flags.z = (registers.D == 0);
                flags.s = (0x80 & registers.D);
                // TODO: implement parity function
                // flags.p = (parity(registers.D));

                pc++;
            }
            break;
        case 0x16:
            // MVI D, byte
            // Load next byte into register D
            {
                registers.D = memory[pc + 1];
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
                pc++;
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
            // Decrement registers D and E, no flags affected
            {
                registers.D--;
                registers.E--;
                pc++;
            }
            break;
        case 0x1c:
            // INC E
            // Increment register E and
            {
                registers.E++;
                // set z, s, and p flags
                pc++;
            }
            break;
        case 0x1d:
            // DEC E
            // Decrement register E and
            {
                registers.E--;
                // set z, s, and p flags
                pc++;
            }
            break;
        case 0x1e:
            // MVI E, byte
            // Load next byte into register E
            {
                // code
                pc++;
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
                uint16_t mem_addr = (registers.H << 8) | (registers.L);
                registers.E = memory[mem_addr];
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

        // 0x90 - 0x9f
        case 0x90:
            // SUB B
            // Subtract register B from register A and store result in A
            {
                uint16_t num1 = registers.A;
                uint16_t num2 = ~registers.B & 0x00ff;
                uint16_t result = num1 + num2 + 0x0001;
                registers.A = result & 0x00ff;

                // Set flags
                flags.z = (registers.A == 0);
                flags.s = (registers.A & 0x80);
                // TODO: implement parity function
                // flags.p = parity(registers.A);
                // If there was a carryout, cy is *RESET*
                flags.cy = !(result & 0x0100);

                pc++;
            }
            break;
            // cases 0x91-0x97 are also SUB instructions

        case 0xd0:
            // RNC
            // Return if no carry
            {
                if (!flags.cy)
                {
                    // pop stack to pc
                }
                else
                {
                    pc++;
                }
            }
            break;
        case 0xd1:
            // POP D
            // Pop top of stack to register D
            {
                // code
                pc++;
            }
            break;
        case 0xd2:
            // JNC
            // Jump if no carry
            {
                if (!flags.cy)
                {
                    uint16_t addr_high = memory[pc + 2];
                    uint16_t addr_low = memory[pc + 1];
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
                    // push address to stack
                    uint16_t addr_high = memory[pc + 2];
                    uint16_t addr_low = memory[pc + 1];
                    pc = (addr_high << 8) | addr_low;
                }
                else
                {
                    pc += 3;
                }
            }
            break;
        case 0xd5:
            // PUSH D
            {
                // code
                pc++;
            }
            break;
        case 0xd6:
            // SUI
            // Subtract immediate from A
            {
                uint16_t num1 = registers.A;
                uint16_t num2 = ~memory[pc + 1] & 0x00ff;
                uint16_t result = num1 + num2 + 0x0001;
                registers.A = result & 0x00ff;

                // Set flags
                flags.z = (registers.A == 0);
                flags.s = (registers.A & 0x80);
                // TODO: implement parity function
                // flags.p = parity(registers.A);
                // If there was a carryout, cy is *RESET*
                flags.cy = !(result & 0x0100);

                pc++;
            }
            break;
        case 0xd7:
            // RST 2
            // calls program at address 0x0010
            {
                // push current address onto stack?
                pc = 0x0010;
            }
            break;
        case 0xd8:
            // RC
            // Return if carry
            {
                if (flags.cy)
                {
                    // return
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
                pc++;
            }
            break;
        case 0xda:
            // JC
            // Jump if carry
            {
                if (flags.cy)
                {
                    uint16_t addr_high = memory[pc + 2];
                    uint16_t addr_low = memory[pc + 1];
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
                    // push address to stack
                    uint16_t addr_high = memory[pc + 2];
                    uint16_t addr_low = memory[pc + 1];
                    pc = (addr_high << 8) | addr_low;
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
                pc++;
            }
            break;
        case 0xde:
            // SBI
            // Subtract immediate from accumulator with borrow
            {
                // code
            }
            break;
        case 0xdf:
            // RST 3
            // Calls program at address 0x0018
            {
                // push current address onto stack?
                pc = 0x0018;
            }
            break;

        default:
            // unknown instruction
            break;
        }
        pc += opbytes;
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
