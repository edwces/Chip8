#pragma once
#include <cstdint>

const unsigned int REGISTER_SIZE = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int KEYPAD_SIZE = 16;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_SIZE = VIDEO_WIDTH * VIDEO_HEIGHT;
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
    uint16_t opcode;

    // OPCODES
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
    void OP_Annn();
    void OP_Bnnn();
    void OP_Dxyn();
    void OP_Cxkk();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
    void OP_NULL();

    // FUNCTION TABLES
    void Table0();
    void Table8();
    void TableE();
    void TableF();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

public:
    Chip8();
    uint8_t keypad[KEYPAD_SIZE]{};
    uint32_t video[VIDEO_SIZE]{};
    void LoadROM(const char *filename);
    void Tick();
};