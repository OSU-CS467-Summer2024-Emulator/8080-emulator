#include <iostream>
#include "emulator.h"

int main(){
    
    Emulator e;

    e.LoadRom("../space_invaders_rom/invaders");
    e.PrintRegisters();
    e.PrintFlags();

    return 0;
}