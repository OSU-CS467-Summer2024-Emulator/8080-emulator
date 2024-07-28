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
    SDL();
    ~SDL();
    void Initialize();
    void DrawGraphic();

public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    unsigned char *memory;
};

#endif // SDLPLATFORM_SDLPLATFORM_HPP_