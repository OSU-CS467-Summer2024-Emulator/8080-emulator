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

int Emulator::parity(int x, int size=8){
    int p = 0;
    x = (x & ((1<<size)-1));
    for (int i = 0; i<size; i++)
	{
		if (x & 0x1){ 
            p++;
        }
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

void Emulator::Emulate(){
    int count = 0;
    pc = 0;

    while (count < 50){
        int opbytes = 1;
        unsigned char opcode = memory[pc];

        opbytes = Disassemble((char*)memory, pc);

        switch (opcode){
            case 0x00: // NOP
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

            case 0x31: //LXI	SP,word
                    {
                        sp = (memory[pc + 2] << 8) | memory[pc + 1];
                        // pc += 2; increment pc here or in dissasembler?
                    }
                    break;
            case 0x32: //STA    (word)
                    {
                        unsigned short offset = (memory[pc + 2] << 8) | memory[pc + 1];
                        memory[offset] = registers.A;
                        // pc += 2;
                    }
                    break;
            case 0x36: //MVI	M,byte
                    {
                        unsigned short offset = (registers.H<<8) | (registers.L);
                        memory[offset] = memory[pc + 1];
                        // pc++;
                    }
            case 0x3a: //LDA    (word)
                    {
                        unsigned short offset = (memory[pc + 2] << 8) | memory[pc + 1];
                        registers.A = memory[offset];
                        // pc += 2:
                    }
                    break;
            case 0x3e: //MVI    A,byte
                    {
                        registers.A = memory[pc + 1];
                        // pc++;
                    }
                    break;
            
            
            case 0x77: //MOV    M,A
                    {
                        unsigned short offset = (registers.H<<8) | (registers.L);
                        memory[offset] = registers.A;
                    }
                    break;
            case 0x7a: //MOV A,D
                    {
                        registers.A = registers.D;
                    }
                    break;
            case 0x7b: //MOV A,E
                    {
                        registers.A = registers.E;
                    }
                    break;
            case 0x7c: //MOV A,H
                    {
                        registers.A = registers.H;
                    }
                    break;
            case 0x7e: //MOV A,HL
                    {
                        unsigned short offset = (registers.H<<8) | (registers.L);
                        registers.A = memory[offset];
                    }
                    break;
            
            
            case 0xf1: //POP PSW
                    {
                        registers.A = memory[sp + 1];
                        uint8_t psw = memory[sp];
                        flags.z = (0x01 == (psw & 0x01));
                        flags.s = (0x02 == (psw & 0x02));
                        flags.p = (0x04 == (psw & 0x04));
                        flags.cy = (0x08 == (psw & 0x08)); // (0x05 == (psw & 0x08)) in reference. Typo? Equates to always false
                        flags.ac = (0x10 == (psw & 0x10));
                        sp += 2;
                    }
                    break;
            case 0xf5: //PUSH PSW
                    {
                        memory[sp - 1] = registers.A;
                        uint8_t psw = (flags.z | flags.s << 1 | flags.p << 2 | flags.cy << 3 | flags.ac << 4 );
                        memory[sp - 2] = psw;
                        sp -= 2;
                    }
                    break;
            case 0xfb: //EI
                    {
                        //interupt_enable = 1; Interupt enable variable not established in Emulator class yet
                    }
                    break;
            case 0xfe: //CPI  byte
                    {
                        uint8_t mem = registers.A - memory[pc + 1];
                        flags.z = (mem == 0);
                        flags.s = (0x80 == (mem & 0x80));
                        flags.p = parity(mem);
                        flags.cy = registers.A < memory[pc + 1];
                        // pc++;
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