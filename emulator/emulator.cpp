#include <iostream>
#include <iomanip>
#include <fstream>
#include "emulator.h"

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

    while (count < 100){
        int opbytes = 1;
        unsigned char opcode = memory[pc];

        switch (opcode){
            case 0x00:
                // NOP
                break;
            case 0x01:

                break;
            case 0x02:

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

            default:
                // unknown instruction
                break;
        }
        pc += opbytes;
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
