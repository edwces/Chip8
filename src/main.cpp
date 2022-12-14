#include <iostream>
#include <stdio.h>
#include <chrono>
#include "chip8.hpp"
#include "platform.hpp"

const unsigned int DEFAULT_VIDEO_SCALE = 10;
const unsigned int DEFAULT_TICK_DELAY = 4;

int main(int argc, char **argv)
{
    // handle Args
    if (argc == 1)
    {
        std::cerr << "ERROR: Missing ROM argument" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Create Chip8 Machine
    Platform platform("Chip8", VIDEO_WIDTH * DEFAULT_VIDEO_SCALE, VIDEO_HEIGHT * DEFAULT_VIDEO_SCALE, VIDEO_WIDTH, VIDEO_HEIGHT);
    std::cout << "DEBUG: CREATED PLATFORM" << std::endl;
    Chip8 chip8;
    std::cout << "DEBUG: CREATED CHIP8 INTERPRETER" << std::endl;

    // Load ROM file
    try
    {
        chip8.LoadROM(argv[1]);
        std::cout << "LOG: Succesfully loaded file" << std::endl;
    }
    catch (const char *message)
    {
        std::cout << message << std::endl;
    }

    // Initialize Main Loop vars
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
    auto lastTick = std::chrono::high_resolution_clock::now();
    bool quit = false;

    // MAIN loop
    while (!quit)
    {
        quit = platform.ProccessEvents(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTick).count();

        if (deltaTime > DEFAULT_TICK_DELAY)
        {
            lastTick = currentTime;
            chip8.Tick();
            platform.Update(chip8.video, videoPitch);
        };
    };
    return 0;
}