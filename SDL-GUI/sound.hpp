#ifndef SDL_GUI_SOUND_HPP_
#define SDL_GUI_SOUND_HPP_

#include <SDL2/SDL.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

class Sound
{
public:
    SDL_AudioSpec m_audioSpec;
    Uint8 *m_data;
    Uint32 m_waveLength;
    SDL_AudioDeviceID m_device;
    const char *path;

    explicit Sound(const char *path);
    void LoadSound();
    ~Sound();
};

#endif // SDL_GUI_SOUND_HPP_
