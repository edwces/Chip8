#include "chip8.hpp"
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <iostream>

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

/// @brief Load ROM instructions to the Chip8 memory
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

/// @brief Clear Screen by setting all bytes to 0
void Chip8::OP_00E0()
{
    std::memset(video, 0, sizeof(video));
};

/// @brief Set random number between 0-255 to Vx
void Chip8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t randByte = rand() % (255 - 0 + 1) + 0;
    registers[Vx] = randByte;
};

/// @brief End soubroutine and return to PC in call stack
void Chip8::OP_00EE()
{
    pc = stack[sp];
    sp -= 1;
};

/// @brief Set PC to given address
void Chip8::OP_1nnn()
{
    uint16_t address = (opcode & 0x0FFFu);
    pc = address;
};

/// @brief Set PC to given address as a subroutine call
void Chip8::OP_2nnn()
{
    sp += 1;
    stack[sp] = pc;
    uint16_t address = (opcode & 0x0FFFu);
    pc = address;
};

/// @brief If Vx and kk are equal skip next instruction
void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t byte = (opcode & 0x00FFu);
    if (registers[Vx] == byte)
    {
        pc += 2;
    }
};

/// @brief If Vx and kk are not equal skip next instruction
void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t byte = (opcode & 0x00FFu);
    if (registers[Vx] != byte)
    {
        pc += 2;
    }
}

/// @brief if Vx and Vy are equal skip next instruction
void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
};

/// @brief set Vx to byte
void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t byte = (opcode & 0x00FFu);
    registers[Vx] = byte;
};

/// @brief Add byte to Vx
void Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t byte = (opcode & 0x00FFu);
    registers[Vx] += byte;
};

/// @brief Set Vx to Vy
void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    registers[Vx] = registers[Vy];
};

/// @brief Set Vx to Vx OR Vy
void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    registers[Vx] |= registers[Vy];
};

/// @brief Set Vx to Vx AND Vy
void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    registers[Vx] &= registers[Vy];
};

/// @brief Set Vx to Vx XOR Vy
void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    registers[Vx] ^= registers[Vy];
};

/// @brief Add Vy to Vx and if sum overflows set VF = 1
void Chip8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    uint16_t sum = Vx + Vy;
    if (sum > 255u)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] = sum & 0xFFu;
};

/// @brief Subtract Vy from Vx and if Vx is bigger than Vy set
///        VF to 1 else to 0
void Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] -= Vy;
};

/// @brief Divide Vx by two and if least significant bit of Vx is 1,
///        then VF is set to 1 else 0
void Chip8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u);
    registers[0xF] = (registers[Vx] & 0x1u);
    // we divide by bitwise operatios as normal division would transform value to int
    registers[Vx] >>= 2;
};

/// @brief If Vy > Vx then VF = 1 and subtract Vx from Vy
///        and set that on Vx
void Chip8::OP_8xy7()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);

    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] = registers[Vy] - registers[Vx];
};

/// @brief If most significant Bit Vx is 1 then VF = 1 else VF = 0
///        then multiply Vx by two
void Chip8::OP_8xyE()
{
    uint8_t Vx = (opcode & 0x0F00u);
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
};

/// @brief Skip next instructions if Vx != Vy
void Chip8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);

    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }
};

/// @brief Execute Current instruction in memory
void Chip8::Parse()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    std::cout << "----------PARSE----------" << std::endl
              << "Current opcode: " << std::hex << opcode << std::endl
              << "Current PC: " << pc << std::endl
              << std::endl;

    pc += 2;
};