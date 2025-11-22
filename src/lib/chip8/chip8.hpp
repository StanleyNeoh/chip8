#ifndef SRC_CHIP8_HPP
#define SRC_CHIP8_HPP

#include <vector>
#include <iostream>
#include <memory>
#include <cstring>
#include <fstream>

#define MEM_SIZE 4096
#define FONT_START 0x050
#define MEM_START 0x200
#define N_REG 16

class Inst;

class Chip8 {
private:
    uint8_t memory[MEM_SIZE]{}; // 4096 bytes RAM
    uint16_t pc = MEM_START;
    uint16_t I = 0;
    uint16_t stack[16]{};
    uint8_t sp = 0;
    uint8_t delay = 0;
    uint8_t sound = 0;
    uint8_t V[N_REG]{}; // V0..VF
    uint16_t rom_end = MEM_START;

public:
    Chip8() { setFont(); }
    void setFont();
    bool loadRom(const std::string& path);
    bool finished() const {
        return pc >= rom_end;
    };
    void step(std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
    void quit() {};

    friend class Inst;
};

#endif