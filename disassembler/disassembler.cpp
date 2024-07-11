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
        case 0x00: 
            cout << "NOP" << endl; 
            break;
        case 0xc3: 
            cout << "JMP $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3;
            break;

        case 0x30: 
            cout << "NOP" << endl; 
            break;
		case 0x31: 
            cout << "LXI SP,#$" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3; 
            break;
		case 0x32:  
            cout << "STA $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3; 
            break;
		case 0x33: 
            cout << "INX SP" << endl;
            break;
        case 0x34: 
            cout << "INR M" << endl;
            break;
		case 0x35: 
            cout << "DCR M" << endl;
            break;
        case 0x36: 
            cout << "MVI M,#$" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl; 
            opbytes=2; 
            break;
        case 0x37: 
            cout << "STC" << endl;
            break;
        case 0x38: 
            cout << "NOP" << endl; 
            break;
        case 0x39: 
            cout << "DAD SP" << endl; 
            break;
        case 0x3a: 
            cout << "LDA $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3; 
            break;
        case 0x3b: 
            cout << "DCX SP" << endl;
            break;
		case 0x3c: 
            cout << "INR A" << endl;
            break;
		case 0x3d: 
            cout << "DCR A" << endl;
            break;
        case 0x3e:  
            cout << "MVI A,#$" << hex 
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 2; 
            break;
		case 0x3f: 
            cout << "CMC" << endl;
            break;

        case 0x70: 
            cout << "MOV M,B" << endl; 
            break;
		case 0x71: 
            cout << "MOV M,C" << endl; 
            break;
		case 0x72: 
            cout << "MOV M,D" << endl; 
            break;
		case 0x73: 
            cout << "MOV M,E" << endl; 
            break;
		case 0x74: 
            cout << "MOV M,H" << endl; 
            break;
		case 0x75: 
            cout << "MOV M,L" << endl; 
            break;
		case 0x76: 
            cout << "HLT" << endl;
            break;
		case 0x77: 
            cout << "MOV M,A" << endl; 
            break;
		case 0x78: 
            cout << "MOV A,B" << endl; 
            break;
		case 0x79: 
            cout << "MOV A,C" << endl; 
            break;
		case 0x7a: 
            cout << "MOV A,D" << endl; 
            break;
		case 0x7b: 
            cout << "MOV A,E" << endl; 
            break;
		case 0x7c: 
            cout << "MOV A,H" << endl; 
            break;
		case 0x7d: 
            cout << "MOV A,L" << endl; 
            break;
		case 0x7e: 
            cout << "MOV A,M" << endl; 
            break;
		case 0x7f: 
            cout << "MOV A,A" << endl; 
            break;

        case 0xb0: 
            cout << "ORA B" << endl; 
            break;
		case 0xb1: 
            cout << "ORA C" << endl; 
            break;
		case 0xb2: 
            cout << "ORA D" << endl; 
            break;
		case 0xb3: 
            cout << "ORA E" << endl; 
            break;
		case 0xb4: 
            cout << "ORA H" << endl; 
            break;
		case 0xb5: 
            cout << "ORA L" << endl; 
            break;
		case 0xb6: 
            cout << "ORA M" << endl; 
            break;
		case 0xb7: 
            cout << "ORA A" << endl; 
            break;
		case 0xb8: 
            cout << "CMP B" << endl; 
            break;
		case 0xb9: 
            cout << "CMP C" << endl; 
            break;
		case 0xba: 
            cout << "CMP D" << endl; 
            break;
		case 0xbb: 
            cout << "CMP E" << endl; 
            break;
		case 0xbc: 
            cout << "CMP H" << endl; 
            break;
		case 0xbd: 
            cout << "CMP L" << endl; 
            break;
		case 0xbe: 
            cout << "CMP M" << endl; 
            break;
		case 0xbf: 
            cout << "CMP A" << endl; 
            break;
        
        case 0xf0: 
            cout << "RP" << endl;  
            break;
		case 0xf1: 
            cout << "POP PSW" << endl; 
            break;
		case 0xf2: 
            cout << "JP $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 3; 
            break;
		case 0xf3: 
            cout << "DI" << endl;  
            break;
		case 0xf4: 
            cout << "CP $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 3; 
            break;
		case 0xf5: 
            cout << "PUSH PSW" << endl;
            break;
		case 0xf6: 
            cout << "ORI #$" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 2; 
            break;
		case 0xf7: 
            cout << "RST 6" << endl;
            break;
		case 0xf8: 
            cout << "RM" << endl;
            break;
		case 0xf9: 
            cout << "SPHL" << endl;
            break;
		case 0xfa: 
            cout << "JM $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 3; 
            break;
		case 0xfb: 
            cout << "EI" << endl;
            break;
		case 0xfc: 
            cout << "CM $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 3; 
            break;
		case 0xfd: 
            cout << "CALL $" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[2]) 
            << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 3; 
            break;
		case 0xfe: 
            cout << "CPI #$" << hex << setfill('0') << setw(2)
            << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 2;
            break;
		case 0xff: 
            cout << "RST 7" << endl;
            break;
        default: 
            cout << "NOT IMPLEMENTED YET" << endl; 
    }
    return opbytes;
}

int main (int argc, char**argv) {
    cout << "Starting Disassembler\n";

    streampos size;
    char * memblock;

    ifstream file (argv[1], ios::in|ios::binary|ios::ate);
    if (file.is_open()) {
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

    while (pc < size) {
        pc += Disassemble(memblock, pc);
    }

    delete[] memblock;
    return 0;
}

