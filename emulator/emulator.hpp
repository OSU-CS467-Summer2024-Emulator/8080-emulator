#ifndef EMULATOR_EMULATOR_HPP_
#define EMULATOR_EMULATOR_HPP_

#include <string>
#include <cstdint>

class Emulator
{
    public:
        Emulator();
        ~Emulator();

        int LoadRom(std::string);

        bool parity(int, int);
        void LogicFlagsA();
        void ArithFlagsA(uint16_t res);
        void ZSPFlags(uint8_t value);

        void WriteToMem(uint16_t address, uint8_t value);
        uint8_t ReadFromHL();
        void WriteToHL(uint8_t value);

        void Push(uint8_t high, uint8_t low);
        void Pop(uint8_t *high, uint8_t *low);

        void UnimplementedInstruction();

        void Emulate();

        void PrintRegisters();
        void PrintFlags();

        void SubtractFromA(uint8_t);

    private:
        struct Registers
        {
            uint8_t A = 0;
            uint8_t B = 0;
            uint8_t C = 0;
            uint8_t D = 0;
            uint8_t E = 0;
            uint8_t H = 0;
            uint8_t L = 0;
        } registers;

        struct Flags
        {
            bool z = 0;  // zero
            bool s = 0;  // sign
            bool p = 0;  // parity
            bool cy = 0; // carry
            bool ac = 0; // auxiliary carry - not needed for space invaders
        } flags;

        // stack pointer
        uint16_t sp;

        // program counter
        uint16_t pc;

        // array for memory
        uint8_t* memory;
};

#endif // EMULATOR_EMULATOR_HPP_
