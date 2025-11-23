#include "instructions.hpp"
#include "../chip8/chip8.hpp"
#include <cstdint>
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
void Inst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    std::cerr << "Base Inst::execute() called - this should not happen\n";
}

void ClearScreen::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    std::fill(frame_buffer.begin(), frame_buffer.end(), 0xFF000000u);
}

void ReturnInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    if (sp(chip8) == 0) throw std::runtime_error("Stack underflow on RET instruction");
    sp(chip8)--;
    pc(chip8) = stack(chip8)[sp(chip8)];
}

void JumpInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    pc(chip8) = inst & 0x0FFF;
}

void SubroutInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    if (sp(chip8) >= 16) throw std::runtime_error("Stack overflow on CALL instruction");
    stack(chip8)[sp(chip8)] = pc(chip8);
    sp(chip8)++;
    pc(chip8) = inst & 0x0FFF;
}

void SkipConstEqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t NN = inst & 0x00FF;
    if (V(chip8)[X] == NN) {
        pc(chip8) += 2;
    }
}

void SkipConstNeqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t NN = inst & 0x00FF;
    if (V(chip8)[X] != NN) {
        pc(chip8) += 2;
    }
}

void SkipRegEqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    if (V(chip8)[X] == V(chip8)[Y]) {
        pc(chip8) += 2;
    }
}

void SkipRegNeqInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    if (V(chip8)[X] != V(chip8)[Y]) {
        pc(chip8) += 2;
    }
} 

void SetConstInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    V(chip8)[(inst >> 8) & 0x0F] = inst & 0x00FF;
}

void AddConstInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    V(chip8)[(inst >> 8) & 0x0F] += inst & 0x00FF;
}

void LoadReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    V(chip8)[X] = V(chip8)[Y];
}

void OrReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    V(chip8)[X] |= V(chip8)[Y];
    V(chip8)[0xF] = 0;
}

void AndReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    V(chip8)[X] &= V(chip8)[Y];
    V(chip8)[0xF] = 0;
}

void XorReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    V(chip8)[X] ^= V(chip8)[Y];
    V(chip8)[0xF] = 0;
}

void AddReg::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    uint16_t sum = static_cast<uint16_t>(V(chip8)[X]) + V(chip8)[Y];
    V(chip8)[X] = sum & 0xFF;
    V(chip8)[0xF] = (sum > 0xFF) ? 1 : 0;
}

void SubXY::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    uint8_t X_val = V(chip8)[X];
    uint8_t Y_val = V(chip8)[Y];
    uint8_t vf = (X_val >= Y_val) ? 1 : 0;
    V(chip8)[X] = X_val - Y_val;
    V(chip8)[0xF] = vf;
}

void SubYX::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    uint8_t X_val = V(chip8)[X];
    uint8_t Y_val = V(chip8)[Y];
    uint8_t vf = (Y_val >= X_val) ? 1 : 0;
    V(chip8)[X] = Y_val - X_val;
    V(chip8)[0xF] = vf;
}

void ShiftRightInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    uint8_t carry = V(chip8)[Y] & 0x01;
    V(chip8)[X] = V(chip8)[Y] >> 1;
    V(chip8)[0xF] = carry;
}

void ShiftLeftInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t Y = (inst >> 4) & 0x0F;
    uint8_t carry = (V(chip8)[Y] & 0x80) >> 7;
    V(chip8)[X] = (V(chip8)[Y] << 1) & 0xFF;
    V(chip8)[0xF] = carry;
} 

void SetIndexInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    I(chip8) = inst & 0x0FFF;
}

void JumpOffsetInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    pc(chip8) = (inst & 0x0FFF) + V(chip8)[0];
}

void RandInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t NN = inst & 0x00FF;
    uint8_t rand_byte = static_cast<uint8_t>(rand() % 256);
    V(chip8)[X] = rand_byte & NN;
}

void DisplayInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
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
                if (pixel != 0xFF000000u) {
                    vf = 1;
                }
                pixel ^= 0x00FFFFFFu;
            }
        }
    }
    V(chip8)[0xF] = vf;
}


void SkipIfKPInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t key = V(chip8)[X];
    if (keydown & (1 << key)) {
        pc(chip8) += 2;
    }
}

void SkipIfNotKPInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t key = V(chip8)[X];
    if (!(keydown & (1 << key))) {
        pc(chip8) += 2;
    }
}

void TimerSetVXInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    V(chip8)[X] = delay(chip8);
}

void TimerSetDelayInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    delay(chip8) = V(chip8)[X];
}

void TimerSetSoundInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    sound(chip8) = V(chip8)[X];
}

void AddIRegInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    I(chip8) += V(chip8)[X];
}

void GetKeyInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    if (keydown == 0) {
        pc(chip8) -= 2; // Repeat this instruction
        return;
    }
    for (uint8_t key = 0; key < 16; ++key) {
        if (keydown & (1 << key)) {
            V(chip8)[X] = key;
            return;
        }
    }
}

void FontCharInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t digit = V(chip8)[X] & 0x0F;
    I(chip8) = 0x50 + (digit * 5);
}

void BinCodedDecConvInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    uint8_t value = V(chip8)[X];
    memory(chip8)[I(chip8) + 2] = value % 10;
    value /= 10;
    memory(chip8)[I(chip8) + 1] = value % 10;
    value /= 10;
    memory(chip8)[I(chip8) + 0] = value % 10;
}

void StoreMemInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    for (uint8_t i = 0; i <= X; ++i) {
        memory(chip8)[I(chip8)++] = V(chip8)[i];
    }
}

void LoadMemInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    uint8_t X = (inst >> 8) & 0x0F;
    for (uint8_t i = 0; i <= X; ++i) {
        V(chip8)[i] = memory(chip8)[I(chip8)++];
    }
}

void UnknownInst::execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
    std::cerr << "Unknown instruction: " << fmt("0x%s", hex(inst, 4)) << "\n";
}
