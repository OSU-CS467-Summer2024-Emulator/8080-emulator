#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
#include "emulator/emulator.hpp"
using namespace std;

int main(int argc, char **argv)
{
  SDL s;
  Emulator e;
  e.LoadRom("./space_invaders_rom/invaders");
  e.Emulate();
  //s.Initialize();
  //s.DrawGraphic();
}