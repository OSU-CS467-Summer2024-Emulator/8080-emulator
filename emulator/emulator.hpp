#ifndef EMULATOR_EMULATOR_HPP_
#define EMULATOR_EMULATOR_HPP_

#include <string>
#include <cstdint>

typedef struct Registers
{
    uint8_t A = 0;
    uint8_t B = 0;
    uint8_t C = 0;
    uint8_t D = 0;
    uint8_t E = 0;
    uint8_t H = 0;
    uint8_t L = 0;
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

    bool interrupt_enable;
    Emulator();
    ~Emulator();

    void AllocateMemory(int size);
    int LoadRom(std::string);

    bool parity(int, int);
    void LogicFlagsA();
    void ArithFlagsA(uint16_t res);
    void ZSPFlags(uint8_t value);

    void SubtractFromA(uint8_t);

    uint8_t ReadFromMem(uint16_t address);
    void WriteToMem(uint16_t address, uint8_t value);
    uint8_t ReadFromHL();
    void WriteToHL(uint8_t value);

    void Call(uint8_t, uint8_t);
    void Return();

    void Push(uint8_t high, uint8_t low);
    void Pop(uint8_t *high, uint8_t *low);

    void InvalidInstruction(uint8_t, uint16_t);

    void Emulate();
    void EmulateOpcode(uint8_t, uint8_t operand1 = 0x00, uint8_t operand2 = 0x00);

    void PrintRegisters();
    void PrintFlags();

    void Interrupt(int interrupt);

    Registers GetRegisters();
    Flags GetFlags();
    int GetPC();
    int GetSP();
    void SetSP(uint16_t);

private:
    Registers registers;

    Flags flags;

    // stack pointer
    uint16_t sp;

    // program counter
    uint16_t pc;

    // array for memory
    uint8_t *memory;
    int mem_size;

    
};

#endif // EMULATOR_EMULATOR_HPP_
