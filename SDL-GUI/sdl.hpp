#ifndef SDLPLATFORM_SDLPLATFORM_HPP_
#define SDLPLATFORM_SDLPLATFORM_HPP_

#include <SDL2/SDL.h>
#include <fstream>
#include <string>
#include <iostream>
#include "./sound.hpp"

using namespace std;

class Emulator;

// create struct to use sounds within gameplay
struct Sounds {
    Sound ufo;
    Sound shoot;
    Sound playerHit;
    Sound invaderHit;
    Sound fleetMv1;
    Sound fleetMv2;
    Sound fleetMv3;
    Sound fleetMv4;
    Sound ufoHit;
    Sounds() : ufo("./audio/0.wav"), shoot("./audio/1.wav"), playerHit("./audio/2.wav"), invaderHit("./audio/3.wav"),
               fleetMv1("./audio/4.wav"), fleetMv2("./audio/5.wav"), fleetMv3("./audio/6.wav"), fleetMv4("./audio/7.wav"),
               ufoHit("./audio/8.wav") {}
};

class SDL
{
public:
    SDL(Emulator &i8080);
    // void Initialize(Emulator* i8080);
    void DrawGraphic();
    void GetInput();
    void RunGame();
    void GetSound();
    void PlaySound(Sound& sound, int pause = 0);
    void LoadSounds();

public:
    SDL_Window *window;
    SDL_Renderer *renderer;
    Emulator &this_cpu;
    Sounds sounds;
    bool ufo_playing = false;
};

#endif // SDLPLATFORM_SDLPLATFORM_HPP_