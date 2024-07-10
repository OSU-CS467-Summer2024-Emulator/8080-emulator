#include <iomanip>
#include <iostream>
#include <locale>
#include <fstream>
using namespace std;


int Disassemble(char *codebuffer, int pc) {    
    char *code = &codebuffer[pc];    
    int opbytes = 1;

    cout << setfill('0') << setw(4) << pc << ' ';
    switch ((unsigned char)*code) {    
        case 0x00: cout << "NOP"; break;
        case 0xc3: printf("JMP    $%02x%02x", (unsigned char)code[2], (unsigned char)code[1]); opbytes = 3; break;
        default: printf("NOT IMPLEMENTED YET"); break;
    }

    cout << "\n";
    return opbytes;
}

int main (int argc, char**argv) {
    cout << "Starting Disassembler\n";

    streampos size;
    char * memblock;

    ifstream file (argv[1], ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char [size];
        file.seekg (0, ios::beg);
        file.read (memblock, size);
        file.close();

        cout << "the entire file content is in memory\n";
    }
    else {
        cout << "Unable to open file" << argv[1];
        exit(1);
    }

    int pc = 0;

    while (pc < 10) {
        pc += Disassemble(memblock, pc);
    }

    delete[] memblock;
    return 0;
}

