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

    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Cxkk();

public:
    Chip8();
    void LoadROM(const char *filename);
    void Parse();
};