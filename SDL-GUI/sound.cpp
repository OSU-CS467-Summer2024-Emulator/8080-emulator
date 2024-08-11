#include <iostream>
#include <SDL2/SDL.h>
#include "./sound.hpp"
#include <fstream>
#include <string>

// Sound class constructor
Sound::Sound(const char *arg) : path(arg), m_data(nullptr){
    
}

// Destructor
Sound::~Sound() {
    SDL_CloseAudioDevice(m_device);
    SDL_FreeWAV(m_data);
}

// Load audio files
void Sound::LoadSound()
{
    if (SDL_LoadWAV(path, &m_audioSpec, &m_data, &m_waveLength) == NULL)
    {
        cout << "sound loading error: " << SDL_GetError() << endl;
        exit(1);
    }
    m_device = SDL_OpenAudioDevice(nullptr, 0, &m_audioSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
}
