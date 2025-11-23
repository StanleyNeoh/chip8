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
    size_t tick = 0;

public:
    Chip8() { setFont(); }
    void setFont();
    bool loadRom(const std::string& path);
    bool finished() const {
        return pc >= rom_end;
    };
    void step(std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown);
    void quit() {};
    bool is_beeping() const { return sound > 0; }

    void memdump(std::ostream& os = std::cout) const {
        os << "pc: " << std::hex << pc << ", I: " << I << ", sp: " << std::dec << static_cast<int>(sp) << "\n";
        os << "V registers:\n";
        for (int i = 0; i < N_REG; ++i) {
            os << "V" << std::hex << i << ": " << std::hex << static_cast<int>(V[i]) << " ";
        }
        os << "\n";
        os << "Memory Dump:\n";
        for (size_t i = 0; i < MEM_SIZE; i += 16) {
            os << std::hex << (i) << ": ";
            for (size_t j = 0; j < 16; ++j) {
                if (i + j < MEM_SIZE) {
                    os << std::hex << static_cast<int>(memory[i + j]) << " ";
                }
            }
            os << "\n";
        }
        os << std::dec << std::endl;
    }

    friend class Inst;
};

#endif