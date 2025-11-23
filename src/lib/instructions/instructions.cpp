#include "instructions.hpp"
#include "../chip8/chip8.hpp"
#include <iostream>

// Inst protected accessor implementations
uint8_t* Inst::memory(Chip8& chip8) { return chip8.memory; }
uint16_t& Inst::pc(Chip8& chip8) { return chip8.pc; }
uint16_t& Inst::I(Chip8& chip8) { return chip8.I; }
uint16_t* Inst::stack(Chip8& chip8) { return chip8.stack; }
uint8_t& Inst::sp(Chip8& chip8) { return chip8.sp; }
uint8_t& Inst::delay(Chip8& chip8) { return chip8.delay; }
uint8_t& Inst::sound(Chip8& chip8) { return chip8.sound; }
uint8_t* Inst::V(Chip8& chip8) { return chip8.V; }

// Base Inst execute - should never be called directly, but needed for vtable
void Inst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    std::cerr << "Base Inst::execute() called - this should not happen\n";
}

void ClearScreen::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    std::fill(frame_buffer.begin(), frame_buffer.end(), 0x00000000u);
}

void ReturnInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void JumpInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    pc(chip8) = inst & 0x0FFF;
}

void SubroutInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SkipConstEqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SkipConstNeqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SkipRegEqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SkipRegNeqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {} 

void SetConstInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    V(chip8)[(inst >> 8) & 0x0F] = inst & 0x00FF;
}

void AddConstInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    V(chip8)[(inst >> 8) & 0x0F] += inst & 0x00FF;
}

void LoadReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void OrReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void AndReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void XorReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void AddReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SubReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void ShiftRightInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SubNReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void ShiftLeftInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {} 

void SetIndexInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    I(chip8) = inst & 0x0FFF;
}

void JumpOffsetInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void RandInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void DisplayInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {
    uint8_t x_corr = V(chip8)[(inst >> 8) & 0x0F] % frame_width;
    uint8_t y_corr = V(chip8)[(inst >> 4) & 0x0F] % frame_height;
    uint8_t vf = 0;
    uint8_t n_rows = inst & 0x0F;
    for (uint8_t row = 0; row < n_rows; ++row) {
        uint8_t sprite_byte = memory(chip8)[I(chip8) + row];
        for (uint8_t col = 0; col < 8; ++col) {
            if ((sprite_byte & (0x80 >> col)) != 0) {
                uint16_t x = (x_corr + col) % frame_width;
                uint16_t y = (y_corr + row) % frame_height;
                uint32_t& pixel = frame_buffer[y * frame_width + x];
                if (pixel != 0x00000000u) {
                    vf = 1;
                }
                pixel ^= 0xFFFFFFFFu;
            }
        }
    }
    V(chip8)[0xF] = vf;
}


void SkipIfKPInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void SkipIfNotKPInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void TimerSetVXInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void TimerSetDelayInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void TimerSetSoundInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {}

void UnknownInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) {} 