#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
#include <fstream>
#include <string>
#include "../emulator/emulator.hpp"

using namespace std;

SDL::SDL(Emulator& i8080) : this_cpu(i8080)
{
    //this_cpu = i8080;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_CreateWindowAndRenderer(224 * 3, 256 * 3, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, 3, 3);
    SDL_RenderSetLogicalSize(renderer, 224, 256);
    cout << "SDL INITIALIZED" << endl;
}

void SDL::DrawGraphic()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for(uint16_t v = 0; v < 224; v++)
    {
        for(int16_t h = 0; h < 256; h++)
        {
            uint16_t base_offset = 0x2400;
            uint16_t vertical_offset = 0x20 * v;
            uint16_t horizontal_offset = (h >> 3);
            uint16_t current_byte = base_offset
                                + vertical_offset
                                + horizontal_offset;
            uint8_t current_bit = (h % 8);

            bool thisPixel = (this_cpu.ReadFromMem(current_byte) & (1 << current_bit)) != 0;

            // retrieve the current pixel color
            if(thisPixel)
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

void SDL::GetInput()
{
    SDL_Event e;
    bool quit = false;
    
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            quit = true;
            this_cpu.PrintRegisters();
            exit(0);
        }

        else if (e.type == SDL_KEYDOWN)
        {
            switch(e.key.keysym.sym) 
            {
                case SDLK_UP:
                    cout << "RIGHT DOWN" << endl;
                    break;
                
                case SDLK_DOWN:
                    cout << "LEFT DOWN" << endl;
                    break;
            }
        }
    }
}

void SDL::RunGame()
{   
    int counter = 0;
    while(true)
    {
        this_cpu.Emulate();
        if (!(++counter % 500))
        {
            printf("------------DRAW GRAPHIC HERE----------------\n");
            // cin.get();
            DrawGraphic();
        }
        // Interrupt 1
        this_cpu.Interrupt(1);

        while (!(this_cpu.interrupt_enable))
        {
            this_cpu.Emulate();
        }
        
        this_cpu.Emulate();

        // Interrupt 2
        this_cpu.Interrupt(2);
        
        while (!(this_cpu.interrupt_enable))
        {
            this_cpu.Emulate();
        }
        
        this_cpu.Emulate();

        GetInput();
        // if(counter % 20 == 0)
        // {
        //     cin.get();
        // }
    }
}