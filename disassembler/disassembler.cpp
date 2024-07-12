#include <iomanip>
#include <iostream>
#include <locale>
#include <fstream>
using namespace std;


int Disassemble(char *codebuffer, int pc) {    
    char *code = &codebuffer[pc];
    int opbytes = 1;

    cout << setfill('0') << setw(4) << pc << ' ' 
         << hex << setfill('0') << setw(2) << static_cast<unsigned>((unsigned char) *code)<< ' ';
    switch ((unsigned char)*code) {    
        case 0x00: 
            cout << "NOP" << endl; 
            break;

        // 0x10 - 0x1f
        case 0x10:
            cout << "NOP" << endl;
            break;
        case 0x11:
            cout << "LXI D" << endl;
            opbytes = 3;
            break;
        case 0x12:
            cout << "STAX D" << endl;
            break;
        case 0x13:
            cout << "INX D" << endl;
            break;
        case 0x14:
            cout << "INR D" << endl;
            break;
        case 0x15:
            cout << "DEC D" << endl;
            break;
        case 0x16:
            cout << "MVI D, $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 2;
            break;
        case 0x17:
            cout << "RAL" << endl;
            break;
        case 0x18:
            cout << "NOP" << endl;
            break;
        case 0x19:
            cout << "DAD D" << endl;
            break;
        case 0x1a:
            cout << "LDAX D" << endl;
            break;
        case 0x1b:
            cout << "DCX D" << endl;
            break;
        case 0x1c:
            cout << "INR E" << endl;
            break;
        case 0x1d:
            cout << "DEC E" << endl;
            break;
        case 0x1e:
            cout << "MVI E, $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes = 2;
            break;
        case 0x1f:
            cout << "RAR" << endl;
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

        // 0x50 - 0x5f
        case 0x50:
            cout << "MOV D, B" << endl;
            break;
        case 0x51:
            cout << "MOV D, C" << endl;
            break;
        case 0x52:
            cout << "MOV D, D (NOP)" << endl;
            break;
        case 0x53:
            cout << "MOV D, E" << endl;
            break;
        case 0x54:
            cout << "MOV D, H" << endl;
            break;
        case 0x55:
            cout << "MOV D, L" << endl;
            break;
        case 0x56:
            cout << "MOV D, M" << endl;
            break;
        case 0x57:
            cout << "MOV D, A" << endl;
            break;
        case 0x58:
            cout << "MOV E, B" << endl;
            break;
        case 0x59:
            cout << "MOV E, C" << endl;
            break;
        case 0x5a:
            cout << "MOV E, D" << endl;
            break;
        case 0x5b:
            cout << "MOV E, E (NOP)" << endl;
            break;
        case 0x5c:
            cout << "MOV E, H" << endl;
            break;
        case 0x5d:
            cout << "MOV E, L" << endl;
            break;
        case 0x5e:
            cout << "MOV E, M" << endl;
            break;
        case 0x5f:
            cout << "MOV E, A" << endl;
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

        // 0x90 - 0x9f
        case 0x90:
            cout << "SUB B" << endl;
            break;
        case 0x91:
            cout << "SUB C" << endl;
            break;
        case 0x92:
            cout << "SUB D" << endl;
            break;
        case 0x93:
            cout << "SUB E" << endl;
            break;
        case 0x94:
            cout << "SUB H" << endl;
            break;
        case 0x95:
            cout << "SUB L" << endl;
            break;
        case 0x96:
            cout << "SUB M" << endl;
            break;
        case 0x97:
            cout << "SUB A" << endl;
            break;
        case 0x98:
            cout << "SBB B" << endl;
            break;
        case 0x99:
            cout << "SUB C" << endl;
            break;
        case 0x9a:
            cout << "SUB D" << endl;
            break;
        case 0x9b:
            cout << "SUB E" << endl;
            break;
        case 0x9c:
            cout << "SUB H" << endl;
            break;
        case 0x9d:
            cout << "SUB L" << endl;
            break;
        case 0x9e:
            cout << "SUB M" << endl;
            break;
        case 0x9f:
            cout << "SUB A" << endl;
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
        
        // 0xd0 - 0xdf
        case 0xd0:
            cout << "RNC" << endl;
            break;
        case 0xd1:
            cout << "POP D" << endl;
            break;
        case 0xd2:
            cout << "JNC $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[2]) 
                 << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3;
            break;
        case 0xd3:
            cout << "OUT $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=2;
            break;
        case 0xd4:
            cout << "CNC $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[2]) 
                 << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3;
            break;
        case 0xd5:
            cout << "PUSH D" << endl;
            break;
        case 0xd6:
            cout << "SUI $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=2;
            break;
        case 0xd7:
            cout << "RST 2 (CALL $0010)" << endl;
            break;
        case 0xd8:
            cout << "RC" << endl;
            break;
        case 0xd9:
            cout << "NOP" << endl;
            break;
        case 0xda:
            cout << "JC $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[2]) 
                 << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3;
            break;
        case 0xdb:
            cout << "IN $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=2;
            break;
        case 0xdc:
            cout << "CC $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[2]) 
                 << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=3;
            break;
        case 0xdd:
            cout << "NOP" << endl;
            break;
        case 0xde:
            cout << "SBI $" << hex << setfill('0') << setw(2)
                 << static_cast<unsigned>((unsigned char)code[1]) << endl;
            opbytes=2;
            break;
        case 0xdf:
            cout << "RST 3 (CALL $0018)" << endl;
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

