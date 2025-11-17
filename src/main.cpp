#include <iostream>
#include <memory>
#include <cstring>
#include <fstream>
#include "ui.hpp"

#define MEM_SIZE 4096
#define FONT_START 0x050
#define MEM_START 0x200
#define N_REG 16

class Chip8 {
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
    void setFont() {
        static const uint8_t fontset[80] = {
            0xF0,0x90,0x90,0x90,0xF0, //0
            0x20,0x60,0x20,0x20,0x70, //1
            0xF0,0x10,0xF0,0x80,0xF0, //2
            0xF0,0x10,0xF0,0x10,0xF0, //3
            0x90,0x90,0xF0,0x10,0x10, //4
            0xF0,0x80,0xF0,0x10,0xF0, //5
            0xF0,0x80,0xF0,0x90,0xF0, //6
            0xF0,0x10,0x20,0x40,0x40, //7
            0xF0,0x90,0xF0,0x90,0xF0, //8
            0xF0,0x90,0xF0,0x10,0xF0, //9
            0xF0,0x90,0xF0,0x90,0x90, //A
            0xE0,0x90,0xE0,0x90,0xE0, //B
            0xF0,0x80,0x80,0x80,0xF0, //C
            0xE0,0x90,0x90,0x90,0xE0, //D
            0xF0,0x80,0xF0,0x80,0xF0, //E
            0xF0,0x80,0xF0,0x80,0x80  //F
        };
        memcpy(memory + 0x050, fontset, 80);
    }

    Chip8() { setFont(); }

    bool initUi(const char* title, int w, int h) {
        return UI::instance().init(title, w, h) == 0;
    }

    bool loadRom(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) return false;
        in.read(reinterpret_cast<char*>(memory + MEM_START), MEM_SIZE - MEM_START);
        rom_end = MEM_START + static_cast<uint16_t>(in.gcount());
        pc = MEM_START;
        return true;
    }

    bool finished() const { return pc >= rom_end; }

    bool step() {
        if (finished()) return false;
        uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
        pc += 2;
        uint16_t nnn = opcode & 0x0FFF;
        uint8_t nn = opcode & 0x00FF;
        uint8_t n = opcode & 0x000F;
        uint8_t x = (opcode >> 8) & 0x0F;
        uint8_t y = (opcode >> 4) & 0x0F;

        switch (opcode & 0xF000) {
            case 0x0000:
                if (opcode == 0x00E0) UI::instance().clear();
                else if (opcode == 0x00EE) { if (sp) pc = stack[--sp]; }
                break;
            case 0x1000: pc = nnn; break;
            case 0x2000: stack[sp++] = pc; pc = nnn; break;
            case 0x6000: V[x] = nn; break;
            case 0x7000: V[x] = static_cast<uint8_t>(V[x] + nn); break;
            case 0xA000: I = nnn; break;
            case 0xD000: {
                uint8_t vx = V[x] & 63; // wrap 0-63
                uint8_t vy = V[y] & 31; // wrap 0-31
                uint8_t collision = 0;
                for (uint8_t row = 0; row < n; ++row) {
                    if (I + row >= MEM_SIZE) break; // safety
                    uint8_t sprite = memory[I + row];
                    for (int bit = 0; bit < 8; ++bit) {
                        if (sprite & (0x80 >> bit)) {
                            int px = (vx + bit) & 63;
                            int py = (vy + row) & 31;
                            if (UI::instance().toggle(py, px)) collision = 1; // pixel turned off
                        }
                    }
                }
                V[0xF] = collision;
                break;
            }
            default: break;
        }
        return true;
    }
};

int main() {
    Chip8 chip8;
    if (!chip8.loadRom("test_opcode.ch8")) {
        std::cerr << "Failed ROM\n";
        return 1;
    }
    if (!chip8.initUi("Chip8", 64, 32)) {
        std::cerr << "Failed UI init\n";
        return 1;
    }
    while (!chip8.finished() && UI::instance().loop()) {
        chip8.step();
        UI::instance().display();
    }
    // Keep window open until user closes it (optional)
    while (UI::instance().loop()) {
        UI::instance().display();
    }
    return 0;
}