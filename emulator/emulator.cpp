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
        cout << "Unable to open file " << file_path << endl;
        return 0;
    }
}

// Determines parity flag
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

void Emulator::UnimplementedInsruction()
{
    cout << "Instruction not implemented" << endl;
    Disassemble((char*)memory, pc);
    cout << endl;
    exit(1);
}

void Emulator::WriteToMem(uint16_t address, uint8_t value) 
{
    if (address < 0x2000 || address >=0x4000)
    {
        cout << "Invalid write location " << address << endl;
        return;
    }
    
    memory[address] = value;
}

void Emulator::WriteToHL(uint8_t value)
{
    uint16_t offset = (registers.H << 8) | registers.L;
    WriteToMem(offset, value);
}

uint8_t Emulator::ReadFromHL()
{
    uint16_t offset = (registers.H << 8) | registers.L;
    return memory[offset];
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
    sp -= 2;
}

void Emulator::ZSPFlags(uint8_t value)
{
    flags.z = (value == 0);
    flags.s =(0x80 == (value & 0x80));
    flags.p = parity(value);
}

void Emulator::Emulate()
{
    int count = 0;
    pc = 0;

    while (count < 100)
    {
        unsigned char opcode = memory[pc];

        Disassemble((char*)memory, pc);

        switch (opcode)
        {
        case 0x00:
            // NOP
            break;
        case 0x01:

            break;
        case 0x02:

            break;

        // 0x30 - 0x3f
        case 0x30:
                UnimplementedInsruction();
                break;
        case 0x31: 
            //LXI SP,word
            {
                sp = (memory[pc + 2] << 8) | memory[pc + 1];
                pc += 3;
            }
            break;
        case 0x32: 
            //STA (word)
            {
                uint16_t offset = (memory[pc + 2] << 8) | memory[pc + 1];
                WriteToMem(offset, registers.A);
                pc += 3;
            }
            break;
        case 0x33: 
            //INX SP
            {
                sp++;
                pc++;
            }
            break;
        case 0x34: 
            //INR M
            {
                uint8_t res = ReadFromHL() + 1;
                ZSPFlags(res);
                WriteToHL(res);
                pc++;
            }
            break;
        case 0x35: 
            //DCR M
            {
                uint8_t res = ReadFromHL() - 1;
                ZSPFlags(res);
                WriteToHL(res);
                pc++;
            }
                break;
        case 0x36: 
            //MVI M, byte
            {
                WriteToHL(memory[pc + 1]);
                pc++;
            }
            break;
        case 0x37: 
            //STC
            {
                flags.cy = 1;
                pc++;
            }
            break;
        case 0x38:
            UnimplementedInsruction();
            break;
        case 0x39: 
            //DAD SP
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
            //LDA (word)
            {
                uint16_t offset = (memory[pc + 2] << 8) | memory[pc + 1];
                registers.A = memory[offset];
                pc += 3;
            }
            break;
        case 0x3b: 
            //DCX SP
            {
                sp -= 1;
                pc++;
            }
            break;
        case 0x3c: 
            //INR A
            { 
                registers.A++;
                ZSPFlags(registers.A);
                pc++;
            }
            break;
        case 0x3d: 
            //DCR A
            {
                registers.A--;
                ZSPFlags(registers.A);
                pc++;
            }
            break;
        case 0x3e: 
            //MVI A, byte
            {
                registers.A = memory[pc + 1];
                pc++;
            }
            break;
        case 0x3f:
            {
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
                int mem_addr = (registers.H << 8) | (registers.L);
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

            // 0x50 - 0x5f
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
                int mem_addr = (registers.H << 8) | (registers.L);
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

        // 0x70 - 0x7f
        case 0x70: 
            //MOV M, B
            {
                WriteToHL(registers.B);
                pc++;
            }
            break;
        case 0x71: 
            //MOV M, C
            {
                WriteToHL(registers.C);
                pc++;
            }
            break;
        case 0x72: 
            //MOV M, D
            {
                WriteToHL(registers.D);
                pc++;
            }
            break;
        case 0x73: 
            //MOV M, E
            {
                WriteToHL(registers.E);
                pc++;
            }
            break;
        case 0x74: 
            //MOV M, H
            {
                WriteToHL(registers.H);
                pc++;
            }
            break;
        case 0x75: 
            //MOV M, L
            {
                WriteToHL(registers.L);
                pc++;
            }
            break;
        case 0x76: 
            //HLT
            {
                pc++;
            }
            break;
        case 0x77: 
            //MOV M, A
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
            //MOV A, D
            {
                registers.A = registers.D;
                pc++;
            }
            break;
        case 0x7b: 
            //MOV A, E
            {
                registers.A = registers.E;
                pc++;
            }
            break;
        case 0x7c: 
            //MOV A, H
            {
                registers.A = registers.H;
                pc++;
            }
            break;
        case 0x7d: 
            //MOV A, L
            {
                registers.A = registers.L;
                pc++;
            }
            break;
        case 0x7e: 
            //MOV A, HL
            {
                registers.A = ReadFromHL();
                pc++;
            }
            break;
        case 0x7f:
            //MOV A, A
            {
                registers.A = registers.A;
                pc++;
            }
            break;

        // 0xb0 - 0xbf
        case 0xb0:
            {
                registers.A = registers.A | registers.B;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb1:
            {
                registers.A = registers.A | registers.C;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb2:
            {
                registers.A = registers.A | registers.D;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb3:
            {
                registers.A = registers.A | registers.E;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb4:
            {
                registers.A = registers.A | registers.H;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb5:
            {
                registers.A = registers.A | registers.L;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb6:
            {
                registers.A = registers.A | ReadFromHL();
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb7:
            {
                registers.A = registers.A | registers.A;
                LogicFlagsA();
                pc++;
            }
            break;
        case 0xb8: //CMP B
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.B;
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xb9: //CMP C
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.C;
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xba: //CMP D
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.D;
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xbb: //CMP E
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.E;
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xbc: //CMP H
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.H;
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xbd: //CMP L
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.L;
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xbe: //CMP HL
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) ReadFromHL();
                ArithFlagsA(res);
                pc++;
            }
            break;
        case 0xbf: //CMP A
            {
                uint16_t res = (uint16_t) registers.A - (uint16_t) registers.A;
                ArithFlagsA(res);
                pc++;
            }
            break;
        
        // 0xf0 - 0xff
        case 0xf0: 
            //RP
            if (flags.s == 0)
            {
                pc = memory[sp] | (memory[sp + 1] << 8);
                sp += 2;
            }
            pc++;
            break;
        case 0xf1: 
            //POP PSW
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
                pc = (memory[pc + 2] << 8) | memory[pc + 1];
            }
            else
            {
                pc += 3;
            }
            break;
        case 0xf3:
            {
                //interupt_enable = 0;
                pc++;
            }
            break;
        case 0xf4: 
            //CP
            if (flags.s == 0)
            {
                uint16_t ret = pc+2;
                WriteToMem(sp - 1, (ret >> 8) & 0xff);
                WriteToMem(sp - 2, (ret & 0xff));
                sp -= 2;
                pc = (memory[pc + 2]) | memory[pc + 1];
            }
            else
            {
                pc += 3;
            }
            break;
        case 0xf5: 
            //PUSH PSW
            {
                memory[sp - 1] = registers.A;
                uint8_t psw = (flags.z | flags.s << 1 | flags.p << 2 | flags.cy << 3 | flags.ac << 4 );
                memory[sp - 2] = psw;
                sp -= 2;
                pc++;
            }
            break;
        case 0xf6: 
            //ORI byte
            {
                uint8_t x = registers.A | memory[pc + 1];
                ZSPFlags(x);
                flags.cy = 0;
                registers.A = x;
                pc++;
            }
            break;
        case 0xf7: 
            // RST 6
            {
                uint16_t ret = pc + 2;
                WriteToMem(sp - 1, (ret >> 8) & 0xff);
                WriteToMem(sp - 2, (ret & 0xff));
                sp -= 2;
                pc = 0x30;
            }
            break;
        case 0xf8: 
            //RM
            if (flags.s != 0)
            {
                pc = memory[sp] | (memory[sp + 1] << 8);
                sp += 2;
            }
            pc++;
            break;
        case 0xf9: 
            //SPHL
            {
                sp = registers.L | (registers.H << 8);
                pc++;
            }
            break;
        case 0xfa: 
            //JM
            if (flags.s != 0)
            {
                pc = (memory[pc + 2] << 8) | memory[pc + 1];
            }
            else
            {
                pc += 3;
            }
            break;
        case 0xfb: 
            //EI
            {
                //interupt_enable = 1; Interupt enable variable not established in Emulator class yet
                pc++;
            }
            break;
        case 0xfc: 
            //CM
            if (flags.s != 0)
            {
                uint16_t ret = pc + 2;
                WriteToMem(sp - 1, (ret >> 8) & 0xff);
                WriteToMem(sp - 2, (ret & 0xff));
                sp -= 2;
                pc = (memory[pc + 2] << 8) | memory[pc + 1];
            }
            else
            {
                pc += 3;
            }
            break;
        case 0xfd: 
            {
                UnimplementedInsruction();
            }
            break;
        case 0xfe: 
            //CPI byte
            {
                uint8_t mem = registers.A - memory[pc + 1];
                ZSPFlags(mem);
                flags.cy = registers.A < memory[pc + 1];
                pc++;
            }
        case 0xff: 
            //RST 7
            {
                uint16_t ret = pc + 2;
                WriteToMem(sp - 1, (ret >> 8) & 0xff);
                WriteToMem(sp - 2, (ret & 0xff));
                sp -= 2;
                pc = 0x38;
            }
            break;
        default:
            // unknown instruction
            break;
        }
        count++;
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
