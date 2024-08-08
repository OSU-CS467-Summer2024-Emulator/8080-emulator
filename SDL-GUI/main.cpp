#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
#include "emulator/emulator.hpp"
using namespace std;

int main(int argc, char **argv)
{
  Emulator e;
  SDL s(e);
  
  s.RunGame();
}