#include "chip8.hpp"
#include <stdio.h>
#include <iostream>

int main()
{
    Chip8 chip8;
    try
    {
        chip8.LoadROM("some");
        std::cout << "Succesfully loaded file" << std::endl;
    }
    catch (const char *message)
    {
        std::cout << message << std::endl;
    }
    return 0;
}