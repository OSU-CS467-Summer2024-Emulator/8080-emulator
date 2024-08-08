#ifndef SDLPLATFORM_SDLPLATFORM_HPP_
#define SDLPLATFORM_SDLPLATFORM_HPP_

#include <SDL2/SDL.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

class Emulator;

class SDL
{
public:
    SDL(Emulator &i8080);
    // void Initialize(Emulator* i8080);
    void DrawGraphic();
    void GetInput();
    void RunGame();
    void GetSound();
    void LoadSound();
    void PlaySound();

public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    Emulator &this_cpu;
};

#endif // SDLPLATFORM_SDLPLATFORM_HPP_