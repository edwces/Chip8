#include "chip8.hpp"
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <iostream>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONSTSET_START_ADDRESS = 0x050;
const unsigned int START_ADDRESS = 0x200;
const unsigned int SCREEN_WIDTH = 64;
const unsigned int SCREEN_HEIGHT = 32;

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
    };

    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    // One unique Tables
    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    // Doubly Unique Tables
    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
};

void Chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
};

void Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL()
{
    std::cout << "DEBUG: "
              << "Unrecognized opcode NULL" << std::endl;
}

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

/// @brief Set Index to address
void Chip8::OP_Annn()
{
    uint16_t address = (opcode & 0x0FFFu);
    index = address;
};

/// @brief Jump to specific address + V0
void Chip8::OP_Bnnn()
{
    uint16_t address = (opcode & 0x0FFFu);
    pc = address + registers[0x0];
};

/// @brief Display n-byte sprite read starting from Index register
///        each sprite has one byte width. We then XOR the screen
///        to check if there is a pixel already in there. If any
///        sprite width or height is to long it wraps itself from
///        opposite site
void Chip8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t Vy = (opcode & 0x00F0u);
    uint8_t height = (opcode & 0x000Fu);

    uint8_t xPos = registers[Vx] % SCREEN_WIDTH;
    uint8_t yPos = registers[Vy] % SCREEN_HEIGHT;

    registers[0xF] = 0;

    // Read Sprite Bytes
    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];
        for (unsigned int column = 0; column < 8; ++column)
        {
            // Read each Byte Pixel and get address of pixel that is already displayed
            uint8_t spritePixel = spriteByte & (0x80u >> column);
            uint32_t *screenPixel = &video[(xPos + column) + (yPos + row) * SCREEN_WIDTH];

            // This works because pointer with zero value is converted to false
            // check if we are drawing anything with sprite pixel
            if (spritePixel)
            {
                // Collision detection
                if (*screenPixel == 0xFFFFFFFFu)
                {
                    registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFFu;
            }
        }
    }
};

/// @brief Skip instruction if key with value of Vx was pressed
void Chip8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t key = registers[Vx];

    if (keypad[key])
    {
        pc += 2;
    }
};

/// @brief Skip instruction if key with value of Vx was not pressed
void Chip8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t key = registers[Vx];

    if (!keypad[key])
    {
        pc += 2;
    }
};

/// @brief Set Vx to delay timer value
void Chip8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u);
    registers[Vx] = delayTimer;
};

/// @brief Wait for key press and then store the value in Vx
void Chip8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u);
    for (uint8_t i = 0; i < KEYPAD_SIZE + 1; ++i)
    {
        if (i == 16)
        {
            pc -= 2;
        }
        if (keypad[i])
        {
            registers[Vx] = i;
        }
    }
};

/// @brief Set delay timer to Vx
void Chip8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u);
    delayTimer = registers[Vx];
};

/// @brief Set sound timer to Vx
void Chip8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u);
    soundTimer = registers[Vx];
};

/// @brief Set Index to Index + Vx
void Chip8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u);
    index += registers[Vx];
};

/// @brief Set Index to location of sprite for digit Vx
void Chip8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint16_t address = FONSTSET_START_ADDRESS + (5 * registers[Vx]);
    index = address;
};

/// @brief Place hundreds digit in Index, tens in Index + 1 and ones in Index + 2
void Chip8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u);
    uint8_t value = registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
};

/// @brief Store registers throught V0 to Vx in memory starting at Index
void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u);

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        memory[index + i] = registers[i];
    }
};

/// @brief Read registers throught V0 to Vx in memory starting at Index into registers
void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u);

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index + i];
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

    ((*this).*(table[(opcode & 0xF000)]))();

    if (delayTimer > 0)
    {
        --delayTimer;
    }
    if (soundTimer > 0)
    {
        --soundTimer;
    }
};