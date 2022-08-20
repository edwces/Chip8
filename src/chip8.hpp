#pragma once
#include <cstdint>

const unsigned int REGISTER_SIZE = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int KEYPAD_SIZE = 16;
const unsigned int SCREEN_SIZE = 64 * 32;
const unsigned int STACK_SIZE = 16;

class Chip8
{
private:
    uint8_t registers[REGISTER_SIZE]{};
    uint8_t memory[MEMORY_SIZE]{};
    uint16_t index;
    uint16_t pc;
    uint16_t stack[STACK_SIZE]{};
    uint8_t sp;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t keypad[KEYPAD_SIZE]{};
    uint32_t video[SCREEN_SIZE]{};
    uint16_t opcode;
    void InstrCLS();
    void InstrRND();
    void InstrRET();
    void InstrJP();
    void InstrCALL();

public:
    Chip8();
    void LoadROM(const char *filename);
    void Loop();
};