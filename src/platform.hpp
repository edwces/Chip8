#pragma once
#include <cstdint>
#include <SDL2/SDL.h>

class Platform
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

public:
    Platform(char const *title, int width, int height, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const *buffer, int pitch);
    bool ProccessEvents(uint8_t *keys);
};