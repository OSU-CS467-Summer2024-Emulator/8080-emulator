#ifndef EMULATOR_EMULATOR_HPP_
#define EMULATOR_EMULATOR_HPP_

#include <string>
#include <cstdint>

typedef struct Registers
{
    unsigned char A = 0;
    unsigned char B = 0;
    unsigned char C = 0;
    unsigned char D = 0;
    unsigned char E = 0;
    unsigned char H = 0;
    unsigned char L = 0;
} Registers;

typedef struct Flags
{
    bool z = 0;  // zero
    bool s = 0;  // sign
    bool p = 0;  // parity
    bool cy = 0; // carry
    bool ac = 0; // auxiliary carry - not needed for space invaders
} Flags;


class Emulator
{
    public:
        Emulator();
        ~Emulator();

        int LoadRom(std::string);

        int parity(int, int);
        void LogicFlagsA();
        void ArithFlagsA(uint16_t res);
        void ZSPFlags(uint8_t value);

        void SubtractFromA(uint8_t);

        void WriteToMem(uint16_t address, uint8_t value);
        uint8_t ReadFromHL();
        void WriteToHL(uint8_t value);

        void Push(uint8_t high, uint8_t low);
        void Pop(uint8_t *high, uint8_t *low);

        void UnimplementedInstruction();

        void Emulate();
        void EmulateOpcode(uint8_t);

        void PrintRegisters();
        void PrintFlags();

        Registers GetRegisters();
        Flags GetFlags();
        int GetPC();
        int GetSP();

    private:
        Registers registers;

        Flags flags;

        // stack pointer
        int sp;

        // program counter
        int pc;

        // array for memory
        unsigned char* memory;
};

#endif // EMULATOR_EMULATOR_HPP_
