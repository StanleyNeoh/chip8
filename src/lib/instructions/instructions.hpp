#ifndef SRC_LIB_INSTRUCTIONS_HPP
#define SRC_LIB_INSTRUCTIONS_HPP

#include <string>
#include <memory>
#include <cstdint>
#include <vector>
#include <iostream>
#include "../utils/format.hpp"
#include "types.hpp"

class Chip8;

// 16-bit instruction type
class Inst {
public:
    inst_t inst;
    Inst(inst_t i): inst(i) {}
    virtual ~Inst() = default;
    virtual const char* cmd() const { return CMD_UNK; }
    virtual std::string desc() const { return DESC_UNK; }
    virtual std::string arg() const { return ""; }
    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) = 0;
    static bool match(inst_t opcode) { return false; }

protected:
    // Protected accessors for Chip8 internals (friend class access)
    static inline uint8_t* memory(Chip8& chip8);
    static inline uint16_t& pc(Chip8& chip8);
    static inline uint16_t& I(Chip8& chip8);
    static inline uint16_t* stack(Chip8& chip8);
    static inline uint8_t& sp(Chip8& chip8);
    static inline uint8_t& delay(Chip8& chip8);
    static inline uint8_t& sound(Chip8& chip8);
    static inline uint8_t* V(Chip8& chip8);
};

template <typename T>
class InstTrait: public Inst {
public:
    InstTrait(inst_t inst): Inst(inst) {}
    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) {
        std::cerr << static_cast<T*>(this)->cmd() << "(" << static_cast<T*>(this)->arg() << ")" << " instruction execution not implemented.\n";
    }

    static bool match(inst_t opcode) {
        return (opcode & T::mask) == T::op;
    }
};

class ClearScreen: public InstTrait<ClearScreen> {
public:
    static const inst_t mask = 0xFFFF;
    static const inst_t op = 0x00E0;
    ClearScreen(inst_t inst): InstTrait<ClearScreen>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_CLS;
    }
    
    virtual std::string desc() const override {
        return DESC_CLS;
    }
    
    virtual std::string arg() const override {
        return "";
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class ReturnInst: public InstTrait<ReturnInst> {
public:
    static const inst_t mask = 0xFFFF;
    static const inst_t op = 0x00EE;

    ReturnInst(inst_t inst): InstTrait<ReturnInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_RET;
    }
    
    virtual std::string desc() const override {
        return DESC_RET;
    }
    
    virtual std::string arg() const override {
        return "";
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class JumpInst: public InstTrait<JumpInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0x1000;

    JumpInst(inst_t inst): InstTrait<JumpInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_JP;
    }
    
    virtual std::string desc() const override {
        return DESC_JP;
    }
    
    virtual std::string arg() const override {
        return fmt("NNN=%s", hex(inst & 0x0FFF, 3));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SubroutInst: public InstTrait<SubroutInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0x2000;

    SubroutInst(inst_t inst): InstTrait<SubroutInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_CALL;
    }
    
    virtual std::string desc() const override {
        return DESC_CALL;
    }
    
    virtual std::string arg() const override {
        return fmt("NNN=%s", hex(inst & 0x0FFF, 3));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SkipConstEqInst: public InstTrait<SkipConstEqInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0x3000;

    SkipConstEqInst(inst_t inst): InstTrait<SkipConstEqInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SE;
    }
    
    virtual std::string desc() const override {
        return DESC_SE;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SkipConstNeqInst: public InstTrait<SkipConstNeqInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0x4000;

    SkipConstNeqInst(inst_t inst): InstTrait<SkipConstNeqInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SNE;
    }
    
    virtual std::string desc() const override {
        return DESC_SNE;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SkipRegEqInst: public InstTrait<SkipRegEqInst> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x5000;

    SkipRegEqInst(inst_t inst): InstTrait<SkipRegEqInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SEV;
    }
    
    virtual std::string desc() const override {
        return DESC_SEV;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SkipRegNeqInst: public InstTrait<SkipRegNeqInst> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x9000;

    SkipRegNeqInst(inst_t inst): InstTrait<SkipRegNeqInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SNEV;
    }
    
    virtual std::string desc() const override {
        return DESC_SNEV;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SetConstInst: public InstTrait<SetConstInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0x6000;

    SetConstInst(inst_t inst): InstTrait<SetConstInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_LDV;
    }
    
    virtual std::string desc() const override {
        return DESC_LDV;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class AddConstInst: public InstTrait<AddConstInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0x7000;

    AddConstInst(inst_t inst): InstTrait<AddConstInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_ADDV;
    }
    
    virtual std::string desc() const override {
        return DESC_ADDV;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class LoadReg: public InstTrait<LoadReg> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8000;

    LoadReg(inst_t inst): InstTrait<LoadReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_LDR;
    }
    
    virtual std::string desc() const override {
        return DESC_LDR;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class OrReg: public InstTrait<OrReg> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8001;

    OrReg(inst_t inst): InstTrait<OrReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_OR;
    }
    
    virtual std::string desc() const override {
        return DESC_OR;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class AndReg: public InstTrait<AndReg> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8002;

    AndReg(inst_t inst): InstTrait<AndReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_AND;
    }
    
    virtual std::string desc() const override {
        return DESC_AND;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class XorReg: public InstTrait<XorReg> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8003;

    XorReg(inst_t inst): InstTrait<XorReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_XOR;
    }
    
    virtual std::string desc() const override {
        return DESC_XOR;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class AddReg: public InstTrait<AddReg> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8004;

    AddReg(inst_t inst): InstTrait<AddReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_ADDR;
    }
    
    virtual std::string desc() const override {
        return DESC_ADDR;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SubXY: public InstTrait<SubXY> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8005;
    SubXY(inst_t inst): InstTrait<SubXY>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SUB;
    }
    
    virtual std::string desc() const override {
        return DESC_SUB;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SubYX: public InstTrait<SubYX> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8007;

    SubYX(inst_t inst): InstTrait<SubYX>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SUBN;
    }
    
    virtual std::string desc() const override {
        return DESC_SUBN;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class ShiftRightInst: public InstTrait<ShiftRightInst> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x8006;

    ShiftRightInst(inst_t inst): InstTrait<ShiftRightInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SHR;
    }
    
    virtual std::string desc() const override {
        return DESC_SHR;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class ShiftLeftInst: public InstTrait<ShiftLeftInst> {
public:
    static const inst_t mask = 0xF00F;
    static const inst_t op = 0x800E;

    ShiftLeftInst(inst_t inst): InstTrait<ShiftLeftInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SHL;
    }
    
    virtual std::string desc() const override {
        return DESC_SHL;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SetIndexInst: public InstTrait<SetIndexInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0xA000;

    SetIndexInst(inst_t inst): InstTrait<SetIndexInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_LDI;
    }
    
    virtual std::string desc() const override {
        return DESC_LDI;
    }
    
    virtual std::string arg() const override {
        return fmt("NNN=%s", hex(inst & 0x0FFF, 3));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class JumpOffsetInst: public InstTrait<JumpOffsetInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0xB000;

    JumpOffsetInst(inst_t inst): InstTrait<JumpOffsetInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_JP0;
    }
    
    virtual std::string desc() const override {
        return DESC_JP0;
    }
    
    virtual std::string arg() const override {
        return fmt("NNN=%s", hex(inst & 0x0FFF, 3));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class RandInst: public InstTrait<RandInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0xC000;

    RandInst(inst_t inst): InstTrait<RandInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_RND;
    }
    
    virtual std::string desc() const override {
        return DESC_RND;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class DisplayInst: public InstTrait<DisplayInst> {
public:
    static const inst_t mask = 0xF000;
    static const inst_t op = 0xD000;

    DisplayInst(inst_t inst): InstTrait<DisplayInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_DRW;
    }
    
    virtual std::string desc() const override {
        return DESC_DRW;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s, N=%s", reg(inst >> 8), reg(inst >> 4), std::to_string(inst & 0xF));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SkipIfKPInst: public InstTrait<SkipIfKPInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xE09E;

    SkipIfKPInst(inst_t inst): InstTrait<SkipIfKPInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SKP;
    }
    
    virtual std::string desc() const override {
        return DESC_SKP;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class SkipIfNotKPInst: public InstTrait<SkipIfNotKPInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xE0A1;

    SkipIfNotKPInst(inst_t inst): InstTrait<SkipIfNotKPInst>(inst) {}

    virtual const char* cmd() const override {
        return CMD_SKNP;
    }
    
    virtual std::string desc() const override {
        return DESC_SKNP;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class TimerSetVXInst: public InstTrait<TimerSetVXInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF007;

    TimerSetVXInst(inst_t inst): InstTrait<TimerSetVXInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_LDDT;
    }
    
    virtual std::string desc() const override {
        return DESC_LDDT;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};
class TimerSetDelayInst: public InstTrait<TimerSetDelayInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF015;

    TimerSetDelayInst(inst_t inst): InstTrait<TimerSetDelayInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_STDT;
    }
    
    virtual std::string desc() const override {
        return DESC_STDT;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class TimerSetSoundInst: public InstTrait<TimerSetSoundInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF018;

    TimerSetSoundInst(inst_t inst): InstTrait<TimerSetSoundInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_STST;
    }
    
    virtual std::string desc() const override {
        return DESC_STST;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class AddIRegInst: public InstTrait<AddIRegInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF01E;
    AddIRegInst(inst_t inst): InstTrait<AddIRegInst>(inst) {}

    virtual const char* cmd() const override {
        return CMD_ADD;
    }
    
    virtual std::string desc() const override {
        return DESC_ADD;
    }

    virtual std::string arg() const override {
        return fmt("I, X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class GetKeyInst: public InstTrait<GetKeyInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF00A;
    GetKeyInst(inst_t inst): InstTrait<GetKeyInst>(inst) {}
    virtual const char* cmd() const override {
        return CMD_KEY;
    }

    virtual std::string desc() const override {
        return DESC_KEY;
    }

    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class FontCharInst: public InstTrait<FontCharInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF029;
    FontCharInst(inst_t inst): InstTrait<FontCharInst>(inst) {}
    virtual const char* cmd() const override {
        return CMD_LDF;
    }
    
    virtual std::string desc() const override {
        return DESC_LDF;
    }

    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class BinCodedDecConvInst: public InstTrait<BinCodedDecConvInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF033;
    BinCodedDecConvInst(inst_t inst): InstTrait<BinCodedDecConvInst>(inst) {}
    virtual const char* cmd() const override {
        return CMD_BCD;
    }

    virtual std::string desc() const override {
        return DESC_BCD;
    }

    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class StoreMemInst: public InstTrait<StoreMemInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF055;
    StoreMemInst(inst_t inst): InstTrait<StoreMemInst>(inst) {}
    virtual const char* cmd() const override {
        return CMD_STR;
    }
    virtual std::string desc() const override {
        return DESC_STR;
    }
    virtual std::string arg() const override {
        return fmt("I, X=%s", reg(inst >> 8));
    }
    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class LoadMemInst: public InstTrait<LoadMemInst> {
public:
    static const inst_t mask = 0xF0FF;
    static const inst_t op = 0xF065;
    LoadMemInst(inst_t inst): InstTrait<LoadMemInst>(inst) {}
    virtual const char* cmd() const override {
        return CMD_LDRM;
    }
    virtual std::string desc() const override {
        return DESC_LDRM;
    }
    virtual std::string arg() const override {
        return fmt("I, X=%s", reg(inst >> 8));
    }
    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

class UnknownInst: public InstTrait<UnknownInst> {
public:
    static const inst_t mask = 0x0000;
    static const inst_t op = 0x0000;

    UnknownInst(inst_t inst): InstTrait<UnknownInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_UNK;
    }
    
    virtual std::string desc() const override {
        return DESC_UNK;
    }
    
    virtual std::string arg() const override {
        return hex(inst, 4);
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height, uint16_t keydown) override;
};

#endif