#ifndef EMULATOR_EMULATOR_HPP_
#define EMULATOR_EMULATOR_HPP_

#include <string>
#include <cstdint>

class Emulator
{
    public:
        Emulator(char* rom_path);
        ~Emulator();

        int LoadRom(std::string);

        int parity(int, int);
        void LogicFlagsA();
        void ArithFlagsA(uint16_t res);
        void ZSPFlags(uint8_t value);

        void WriteToMem(uint16_t address, uint8_t value);
        uint8_t ReadMem(uint16_t address);
        uint8_t ReadFromHL();
        void WriteToHL(uint8_t value);

        void Push(uint8_t high, uint8_t low);
        void Pop(uint8_t *high, uint8_t *low);

        void UnimplementedInstruction();

        void Emulate();

        void PrintRegisters();
        void PrintFlags();

        void SubtractFromA(uint8_t);

        void GenerateInterrupt(int interrupt_num);

        // array for memory
        // unsigned char* memory;
        

    private:
        struct Registers
        {
            unsigned char A = 0;
            unsigned char B = 0;
            unsigned char C = 0;
            unsigned char D = 0;
            unsigned char E = 0;
            unsigned char H = 0;
            unsigned char L = 0;
           
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
        int sp;

        // program counter
        int pc = 0;

        // array for memory
        unsigned char* memory;

         uint8_t interrupt_enable = 0;
        
};

#endif // EMULATOR_EMULATOR_HPP_
