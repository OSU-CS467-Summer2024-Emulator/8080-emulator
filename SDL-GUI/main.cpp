#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char** argv) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window{SDL_CreateWindow(
      "Hello Window", 0, 0, 800, 300, 0)};

    bool quit = false;
    SDL_Event event;
    while (!quit) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = true;
        }
      }
        SDL_Delay(10);
    }

  
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


int loadrom(string file_path)
{
    streampos size;
    char *mem_buffer;
    unsigned char* memory;

    ifstream file(file_path, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memory = new unsigned char[size];

        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char *>(memory), size);
        file.close();

        return size;
    }
    else
    {
        cout << "Unable to open file " << file_path << endl;
        return 0;
    }
}