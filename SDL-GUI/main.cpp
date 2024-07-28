#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
using namespace std;

int main(int argc, char **argv)
{
  SDL s;
  s.Loadrom("./space_invaders_rom/invaders");
  s.Initialize();
  s.DrawGraphic();
}