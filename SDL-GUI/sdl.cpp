#include <iostream>
#include <SDL2/SDL.h>
#include "./sdl.hpp"
#include <fstream>
#include <string>
#include "../emulator/emulator.hpp"

using namespace std;

SDL::SDL(Emulator &i8080) : this_cpu(i8080)
{
    // this_cpu = i8080;
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
    for (uint16_t v = 0; v < 224; v++)
    {
        for (int16_t h = 0; h < 256; h++)
        {
            uint16_t base_offset = 0x2400;
            uint16_t vertical_offset = 0x20 * v;
            uint16_t horizontal_offset = (h >> 3);
            uint16_t current_byte = base_offset + vertical_offset + horizontal_offset;
            uint8_t current_bit = (h % 8);

            bool thisPixel = (this_cpu.ReadFromMem(current_byte) & (1 << current_bit)) != 0;

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

void SDL::GetInput()
{
    SDL_Event e;
    bool quit = false;

    SDL_PollEvent(&e);

    if (e.type == SDL_QUIT)
    {
        quit = true;
        this_cpu.PrintRegisters();
        exit(0);
    }

    else if (e.type == SDL_KEYDOWN)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            cout << "UP ARROW KEY PRESSED (Credit)" << endl;
            this_cpu.SetPort1(0, 1);
            break;
        case SDLK_RETURN:
            cout << "ENTER PRESSED (Player 1 start)" << endl;
            this_cpu.SetPort1(2, 1);
            break;
        case SDLK_SPACE:
            cout << "SPACE PRESSED (Player 1 fire)" << endl;
            this_cpu.SetPort1(4, 1);
            break;
        case SDLK_LEFT:
            cout << "LEFT PRESSED (Player 1 left)" << endl;
            this_cpu.SetPort1(5, 1);
            break;
        case SDLK_RIGHT:
            cout << "RIGHT PRESSED (Player 1 right)" << endl;
            this_cpu.SetPort1(6, 1);
            break;
        }
    }
    else if (e.type == SDL_KEYUP) // key released
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            cout << "UP ARROW KEY RELEASED" << endl;
            this_cpu.SetPort1(0, 0);
            break;
        case SDLK_RETURN:
            cout << "ENTER RELEASED" << endl;
            this_cpu.SetPort1(2, 0);
            break;
        case SDLK_SPACE:
            cout << "SPACE RELEASED" << endl;
            this_cpu.SetPort1(4, 0);
            break;
        case SDLK_LEFT:
            cout << "LEFT RELEASED" << endl;
            this_cpu.SetPort1(5, 0);
            break;
        case SDLK_RIGHT:
            cout << "RIGHT RELEASED" << endl;
            this_cpu.SetPort1(6, 0);
            break;
        }
    }
}

uint8_t last_out_port3 = 0;
uint8_t last_out_port5 = 0;

void SDL::LoadSounds()
{
    // load all sounds for use
    sounds.shoot.LoadSound();
    sounds.invaderHit.LoadSound();
    sounds.playerHit.LoadSound();
    sounds.ufo.LoadSound();
    sounds.fleetMv1.LoadSound();
    sounds.fleetMv2.LoadSound();
    sounds.fleetMv3.LoadSound();
    sounds.fleetMv4.LoadSound();
    sounds.ufoHit.LoadSound();
}

void SDL::PlaySound(uint8_t last_sound, Sound& s)
{
    cout << "Play sound" << endl;
    SDL_QueueAudio(s.m_device, s.m_data, s.m_waveLength);
    SDL_PauseAudioDevice(s.m_device, 0);
}


void SDL::GetSound()
{
    if (this_cpu.GetPorts().port3 != last_out_port3)
    {
        if ((this_cpu.GetPorts().port3 & 0x1) && !(last_out_port3 & 0x1))
        {
            PlaySound(0x1, sounds.ufo);
        }
        if ((this_cpu.GetPorts().port3 & 0x2) && !(last_out_port3 & 0x2))
        {
            PlaySound(0x2, sounds.shoot);
        }
        if((this_cpu.GetPorts().port3 & 0x4) && !(last_out_port3 & 0x4))
        {
            PlaySound(0x4, sounds.playerHit);
        }
        if ((this_cpu.GetPorts().port3 & 0x8) && !(last_out_port3 & 0x8))
        {
            PlaySound(0x8, sounds.invaderHit);
        }
        last_out_port3 = this_cpu.GetPorts().port3;
    }

    if (this_cpu.GetPorts().port5 != last_out_port5)
    {
        if ((this_cpu.GetPorts().port5 & 0x1) && !(last_out_port5 & 0x1))
        {
            PlaySound(0x1, sounds.fleetMv1);
        }
        if ((this_cpu.GetPorts().port5 & 0x2) && !(last_out_port5 & 0x2))
        {
            PlaySound(0x2, sounds.fleetMv2);
        }
        if((this_cpu.GetPorts().port5 & 0x4) && !(last_out_port5 & 0x4))
        {
            PlaySound(0x4, sounds.fleetMv3);
        }
        if ((this_cpu.GetPorts().port5 & 0x8) && !(last_out_port5 & 0x8))
        {
            PlaySound(0x8, sounds.fleetMv4);
        }
        if ((this_cpu.GetPorts().port5 & 0x10) && !(last_out_port5 & 0x10))
        {
            PlaySound(0x8, sounds.ufoHit);
        }
        last_out_port5 = this_cpu.GetPorts().port5;
    }
}

void SDL::RunGame()
{
    int counter = 0;

    // load all sounds for use
    LoadSounds();

    uint32_t lastFrameTime = SDL_GetTicks();
    uint32_t currentTime = SDL_GetTicks();
    int emu_cycles = 16666;

    while (true)
    {
        currentTime = SDL_GetTicks();
        if ((currentTime - lastFrameTime) > 17)
        {
            lastFrameTime = currentTime;

            this_cpu.Emulate(emu_cycles);

            // Interrupt 1
            // cin.get();
            // printf("INTERRUPT 1----------------------------------------------\n");
            GetInput();
            this_cpu.Interrupt(1);
            this_cpu.Emulate(emu_cycles);

            // Interrupt 2
            // printf("INTERRUPT 2----------------------------------------------\n");
            GetInput();
            this_cpu.Interrupt(2);

            DrawGraphic();
        }
        GetInput();
        GetSound();
    }
}