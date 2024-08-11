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
}

// Draw pixels to screen from Space Invaders video RAM
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
                if (h < 64 && (h >= 16 || (v >= 16 && v < 128)))
                {
                    // green in player area
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                }
                else if (h >= 192 && h < 224)
                {
                    // red for UFO area
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                }
                else
                {
                    // white everywhere else
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                }
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

// Handle user input by setting input ports
void SDL::GetInput()
{
    SDL_Event e;

    SDL_PollEvent(&e);

    if (e.type == SDL_QUIT)
    {
        exit(0);
    }
    else if (e.type == SDL_KEYDOWN)
    {
        // key pressed
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            this_cpu.SetPort(1, 0, 1);
            break;
        case SDLK_2:
            this_cpu.SetPort(1, 1, 1);
            break;
        case SDLK_RETURN:
            this_cpu.SetPort(1, 2, 1);
            break;
        case SDLK_SPACE:
            this_cpu.SetPort(1, 4, 1);
            break;
        case SDLK_LEFT:
            this_cpu.SetPort(1, 5, 1);
            break;
        case SDLK_RIGHT:
            this_cpu.SetPort(1, 6, 1);
            break;
        case SDLK_w:
            this_cpu.SetPort(2, 4, 1);
            break;
        case SDLK_a:
            this_cpu.SetPort(2, 5, 1);
            break;
        case SDLK_d:
            this_cpu.SetPort(2, 6, 1);
            break;
        }
    }
    else if (e.type == SDL_KEYUP)
    {
        // key released
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            this_cpu.SetPort(1, 0, 0);
            break;
        case SDLK_2:
            this_cpu.SetPort(1, 1, 0);
            break;
        case SDLK_RETURN:
            this_cpu.SetPort(1, 2, 0);
            break;
        case SDLK_SPACE:
            this_cpu.SetPort(1, 4, 0);
            break;
        case SDLK_LEFT:
            this_cpu.SetPort(1, 5, 0);
            break;
        case SDLK_RIGHT:
            this_cpu.SetPort(1, 6, 0);
            break;
        case SDLK_w:
            this_cpu.SetPort(2, 4, 0);
            break;
        case SDLK_a:
            this_cpu.SetPort(2, 5, 0);
            break;
        case SDLK_d:
            this_cpu.SetPort(2, 6, 0);
            break;
        }
    }
}

// load all sounds for use
void SDL::LoadSounds()
{
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

// Play video game sounds
void SDL::PlaySound(Sound& s, int pause)
{
    SDL_QueueAudio(s.m_device, s.m_data, s.m_waveLength);
    SDL_PauseAudioDevice(s.m_device, pause);
}

// Initialize ports for handlin game audio
uint8_t last_out_port3 = 0;
uint8_t last_out_port5 = 0;

// Determine if output ports are set to play sounds
void SDL::GetSound()
{
    // UFO sound repeats while it is flying across screen
    if ((this_cpu.GetPorts().port3 & 0x1))
        {
            ufo_playing = true;
            PlaySound(sounds.ufo);
        }
    else if (!(this_cpu.GetPorts().port3 & 0x1) && ufo_playing)
        {
            ufo_playing = false;
            PlaySound(sounds.ufo, 1);
        }

    // All other sounds play one time when called
    if (this_cpu.GetPorts().port3 != last_out_port3)
    {   
        if ((this_cpu.GetPorts().port3 & 0x2) && !(last_out_port3 & 0x2))
        {
            PlaySound(sounds.shoot);
        }
        if((this_cpu.GetPorts().port3 & 0x4) && !(last_out_port3 & 0x4))
        {
            PlaySound(sounds.playerHit);
        }
        if ((this_cpu.GetPorts().port3 & 0x8) && !(last_out_port3 & 0x8))
        {
            PlaySound(sounds.invaderHit);
        }
        last_out_port3 = this_cpu.GetPorts().port3;
    }

    if (this_cpu.GetPorts().port5 != last_out_port5)
    {
        if ((this_cpu.GetPorts().port5 & 0x1) && !(last_out_port5 & 0x1))
        {
            PlaySound(sounds.fleetMv1);
        }
        if ((this_cpu.GetPorts().port5 & 0x2) && !(last_out_port5 & 0x2))
        {
            PlaySound(sounds.fleetMv2);
        }
        if((this_cpu.GetPorts().port5 & 0x4) && !(last_out_port5 & 0x4))
        {
            PlaySound(sounds.fleetMv3);
        }
        if ((this_cpu.GetPorts().port5 & 0x8) && !(last_out_port5 & 0x8))
        {
            PlaySound(sounds.fleetMv4);
        }
        if ((this_cpu.GetPorts().port5 & 0x10) && !(last_out_port5 & 0x10))
        {
            PlaySound(sounds.ufoHit);
        }
        last_out_port5 = this_cpu.GetPorts().port5;
    }
}

// Handle interrupts for displaying screen, call emulator,
// get user input, and sound for game
void SDL::RunGame()
{
    // load all sounds for use
    LoadSounds();


    // Used to determine time between display updates
    uint32_t lastFrameTime = SDL_GetTicks();
    uint32_t currentTime = SDL_GetTicks();

    // 16666 cycles allows for the 60 Hz refresh rate
    int emu_cycles = 16666;

    while (true)
    {
        currentTime = SDL_GetTicks();
        if ((currentTime - lastFrameTime) > 17)
        {
            lastFrameTime = currentTime;

            this_cpu.Emulate(emu_cycles);

            // Interrupt 1 to update top half of screen
            GetInput();
            this_cpu.Interrupt(1);
            this_cpu.Emulate(emu_cycles);

            // Interrupt 2 to update bottom half of screen
            GetInput();
            this_cpu.Interrupt(2);

            DrawGraphic();
        }
        GetInput();
        GetSound();
    }
}
