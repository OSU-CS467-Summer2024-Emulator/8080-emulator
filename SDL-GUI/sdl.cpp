#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
#include <fstream>
#include <string>

using namespace std;

SDL::SDL() : window(nullptr), renderer(nullptr), memory(nullptr)
{
}

SDL::~SDL()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    if (memory)
    {
        delete[] memory;
    }
    SDL_Quit();
}

void SDL::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    }
    SDL_CreateWindowAndRenderer(224, 256, 0, &window, &renderer);
}

void SDL::DrawGraphic()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (uint16_t v = 0; v < 224; v++)
    {
        for (int16_t h = 0; h < 256; h++)
        {
            uint16_t base_offset = 0x2400;
            uint16_t vertical_offset = 0x20 * v;
            uint16_t horizontal_offset = (h >> 3);
            uint16_t current_byte = base_offset + vertical_offset + horizontal_offset;
            uint8_t current_bit = (h % 8);

            bool thisPixel = (memory[current_byte] & (1 << current_bit)) != 0;

            // retrieve the current pixel color
            if (thisPixel)
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            }

            // Rotate coordinates counter clockwise
            SDL_RenderDrawPoint(renderer, v, 256 - h - 1);
        }
    }
    SDL_RenderPresent(renderer);
}