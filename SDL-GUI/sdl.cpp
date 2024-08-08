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

void SDL::GetInput()
{
    SDL_Event e;
    bool quit = false;

    SDL_PollEvent(&e);

    if (e.type == SDL_QUIT)
    {
        quit = true;
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

SDL_AudioSpec m_audioSpec;
Uint8 *m_data;
Uint32 m_waveLength;
SDL_AudioDeviceID m_device;
uint8_t last_out_port3 = 0;
char *path = "./collide.wav";

void SDL::LoadSound()
{
    if (SDL_LoadWAV(path, &m_audioSpec, &m_data, &m_waveLength) == NULL)
    {
        cout << "sound loading error: " << SDL_GetError() << endl;
        exit(1);
    }
    m_device = SDL_OpenAudioDevice(nullptr, 0, &m_audioSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
}

void SDL::PlaySound()
{
    cout << "Play sound" << endl;
    SDL_QueueAudio(m_device, m_data, m_waveLength);
    SDL_PauseAudioDevice(m_device, 0);
}

void SDL::GetSound()
{
    if (this_cpu.GetPorts().port3 != last_out_port3)
    {
        if ((this_cpu.GetPorts().port3 & 0x2) && !(last_out_port3 & 0x2))
        {
            PlaySound();
        }
    }
}

void SDL::RunGame()
{
    int counter = 0;

    //LoadSound();

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
            GetInput();
            this_cpu.Interrupt(1);
            this_cpu.Emulate(emu_cycles);

            // Interrupt 2
            GetInput();
            this_cpu.Interrupt(2);

            DrawGraphic();
        }
        GetInput();
        //GetSound();
    }
}