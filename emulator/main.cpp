#include <iostream>
#include "emulator/emulator.hpp"

int main()
{
    Emulator e;

    e.LoadRom("./space_invaders_rom/invaders");
    e.Emulate();
    std::cout << "\nEND EMULATION" << std::endl;
    e.PrintRegisters();
    e.PrintFlags();

    return 0;
}
