#include "chip8.hpp"
#include <fstream>

const unsigned int START_ADDRESS = 0x200;

Chip8::Chip8()
{
    pc = START_ADDRESS;
};

void Chip8::LoadROM(const char *filename)
{
    // Open file as binary and move pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        // Get size of file  allocate buffer to hold a file
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        // go to the start and populate buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // Load content into Memory
        for (long i = 0; i < size; ++i)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
    else
    {
        throw "File could not be opened";
    }
};
