#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
#include "emulator/emulator.hpp"
using namespace std;

int main(int argc, char **argv)
{
  Emulator e("./space_invaders_rom/invaders");
  SDL s(e);
  // Emulator e;
  // e.LoadRom("./space_invaders_rom/invaders");
  // e.Emulate();
  
  // s.Initialize(&e);
  s.RunGame();

  // s.DrawGraphic(&e);
  // s.GetInput();
}