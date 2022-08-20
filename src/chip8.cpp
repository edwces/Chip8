#include "chip8.hpp"
#include <fstream>
#include <cstring>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONSTSET_START_ADDRESS = 0x050;
const unsigned int START_ADDRESS = 0x200;

// 16 chars 5 byte each
uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
    pc = START_ADDRESS;
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONSTSET_START_ADDRESS + i] = fontset[i];
    }
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

// Clear the screen
void Chip8::InstrCLS()
{
    std::memset(video, 0, sizeof(video));
};

// set Random number between 0-255 to given register
void Chip8::InstrRND()
{
    // get the register value from opcode by using AND
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t randByte = rand() % (255 - 0 + 1) + 0;
    registers[Vx] = randByte;
};

// End soubroutine
void Chip8::InstrRET()
{
    pc = stack[sp];
    sp -= 1;
};

// set program counter to given address
void Chip8::InstrJP()
{
    uint16_t address = (opcode & 0x0FFFu);
    pc = address;
};

// initialize given subroutine
void Chip8::InstrCALL()
{
    sp += 1;
    stack[sp] = pc;
    uint16_t address = (opcode & 0x0FFFu);
    pc = address;
};

// FIX: handling opcodes with dynamic values like kk, x, y
void Chip8::Loop()
{
    for (long i = 0; i < MEMORY_SIZE; ++i)
    {
        opcode = memory[START_ADDRESS + i];
        uint8_t shortened = (opcode & 0xF00F);

        switch (opcode)
        {
        case 0x0000u:
            InstrCLS();
            break;
        case 0x000Eu:
            InstrRET();
            break;
        default:
            break;
        };

        pc += 1;
    }
};