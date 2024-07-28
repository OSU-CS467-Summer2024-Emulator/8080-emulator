#ifndef SDLPLATFORM_SDLPLATFORM_HPP_
#define SDLPLATFORM_SDLPLATFORM_HPP_

#include <SDL2/SDL.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

class SDL
{
public:
    void Initialize();
    void DrawGraphic(unsigned char *memory);
    void GetInput();

public:
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif // SDLPLATFORM_SDLPLATFORM_HPP_