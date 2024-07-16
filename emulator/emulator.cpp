#include <iostream>
#include <iomanip>
#include <fstream>
#include "emulator.h"
#include "../disassembler/disassembler.h"

using namespace std;

// Constructor
Emulator::Emulator(){

}


// Destructor
Emulator::~Emulator(){
    // deallocate memory if necessary
    if (memory != nullptr){
        delete [] memory;
    }
}


// Copy contents of file specified by file_path into memory
// Returns number of bytes read
int Emulator::LoadRom(string file_path){
    streampos size;
    char * mem_buffer;

    ifstream file (file_path, ios::in|ios::binary|ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memory = new unsigned char [size];

        file.seekg(0, ios::beg);
        file.read((char *) memory, size);
        file.close();

        return size;
    }
    else {
        cout << "Unable to open file" << file_path << endl;
        return 0;
    }
}


void Emulator::Emulate(){
    int count = 0;
    pc = 0;

    while (count < 50){
        int opbytes = 1;
        unsigned char *opcode = &memory[pc];

        opbytes = Disassemble((char*)memory, pc);

        switch (*opcode){
            case 0x00:
                registers.A++;
                break;
            case 0x01:
                // Option 1: 
                // put 
                // code 
                // here
                opbytes = 2;
                break;
            case 0x02:
                // Option 2:
                opbytes = _0x02();
                break;

            case 0x31:
                    {
                        sp = (opcode[2]<<8) | opcode[1];
                        pc += 2;
                    }
                    break;

            case 0x77:
                    {
                        unsigned short offset = (registers.H<<8) | (registers.L);
                        memory[offset] = registers.A;
                    }
                    break;
            case 0x7a:
                    {
                        registers.A = registers.D;
                    }
                    break;
            case 0x7b:
                    {
                        registers.A = registers.E;
                    }
                    break;
            case 0x7c:
                    {
                        registers.A = registers.H;
                    }
                    break;
            case 0x7e:
                    {
                        unsigned short offset = (registers.H<<8) | (registers.L);
                        registers.A = memory[offset];
                    }
                    break;
            // ...
            default:
                // unknown instruction
                break;
        }
        pc += opbytes;
        count++;
    }
}



// Print contents of all registers
void Emulator::PrintRegisters(){
    cout << "Register A: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.A) << endl;
    cout << "Register B: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.B) << endl;
    cout << "Register C: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.C) << endl;
    cout << "Register D: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.D) << endl;
    cout << "Register E: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.E) << endl;
    cout << "Register H: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.H) << endl;
    cout << "Register L: " << hex << setfill('0') << setw(2)
         << static_cast<unsigned>((unsigned char) registers.L) << endl;
}

// Print current state of all condition codes
void Emulator::PrintFlags(){
    cout << "Zero Flag:      " << flags.z  << endl;
    cout << "Sign Flag:      " << flags.s  << endl;
    cout << "Parity Flag:    " << flags.p  << endl;
    cout << "Carry Flag:     " << flags.cy << endl;
    cout << "Aux Carry Flag: " << flags.ac << endl;
}

int Emulator::_0x02(){
    // put
    // code
    // here
    return 1; // opbytes
}