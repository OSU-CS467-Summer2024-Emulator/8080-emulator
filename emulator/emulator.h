#include <string>


class Emulator{
    public:
        Emulator();
        ~Emulator();

        int LoadRom(std::string);

        void Emulate();

        void PrintRegisters();
        void PrintFlags();

    private:
        struct Registers {
            unsigned char A = 0;
            unsigned char B = 0;
            unsigned char C = 0;
            unsigned char D = 0;
            unsigned char E = 0;
            unsigned char H = 0;
            unsigned char L = 0;
        } registers;

        struct Flags {
            bool z = 0;  // zero
            bool s = 0;  // sign
            bool p = 0;  // parity
            bool cy = 0; // carry
            bool ac = 0; // auxiliary carry - not needed for space invaders
        } flags;

        // stack pointer
        int sp;

        // program counter
        int pc;

        // array for memory
        unsigned char* memory;

        int _0x02();
};

