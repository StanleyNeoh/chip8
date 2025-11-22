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
    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);

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
    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height) override {
        std::cerr << static_cast<T*>(this)->cmd() << "(" << static_cast<T*>(this)->arg() << ")" << " instruction execution not implemented.\n";
    }
};

class ClearScreen: public InstTrait<ClearScreen> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class ReturnInst: public InstTrait<ReturnInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class JumpInst: public InstTrait<JumpInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SubroutInst: public InstTrait<SubroutInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SkipConstEqInst: public InstTrait<SkipConstEqInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SkipConstNeqInst: public InstTrait<SkipConstNeqInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SkipRegEqInst: public InstTrait<SkipRegEqInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SkipRegNeqInst: public InstTrait<SkipRegNeqInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SetConstInst: public InstTrait<SetConstInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class AddConstInst: public InstTrait<AddConstInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class LoadReg: public InstTrait<LoadReg> {
public:
    static const inst_t op = 0x0000;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class OrReg: public InstTrait<OrReg> {
public:
    static const inst_t op = 0x0001;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class AndReg: public InstTrait<AndReg> {
public:
    static const inst_t op = 0x0002;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class XorReg: public InstTrait<XorReg> {
public:
    static const inst_t op = 0x0003;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class AddReg: public InstTrait<AddReg> {
public:
    static const inst_t op = 0x0004;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SubReg: public InstTrait<SubReg> {
public:
    static const inst_t op = 0x0005;
    SubReg(inst_t inst): InstTrait<SubReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SUB;
    }
    
    virtual std::string desc() const override {
        return DESC_SUB;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class ShiftRightInst: public InstTrait<ShiftRightInst> {
public:
    static const inst_t op = 0x0006;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SubNReg: public InstTrait<SubNReg> {
public:
    static const inst_t op = 0x0007;
    SubNReg(inst_t inst): InstTrait<SubNReg>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SUBN;
    }
    
    virtual std::string desc() const override {
        return DESC_SUBN;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, Y=%s", reg(inst >> 8), reg(inst >> 4));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class ShiftLeftInst: public InstTrait<ShiftLeftInst> {
public:
    static const inst_t op = 0x000E;
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SetIndexInst: public InstTrait<SetIndexInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class JumpOffsetInst: public InstTrait<JumpOffsetInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class RandInst: public InstTrait<RandInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class DisplayInst: public InstTrait<DisplayInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class SkipIfKeyInst: public InstTrait<SkipIfKeyInst> {
public:
    SkipIfKeyInst(inst_t inst): InstTrait<SkipIfKeyInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_SKP;
    }
    
    virtual std::string desc() const override {
        return DESC_SKP;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s", reg(inst >> 8));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class TimerInst: public InstTrait<TimerInst> {
public:
    TimerInst(inst_t inst): InstTrait<TimerInst>(inst) {}
    
    virtual const char* cmd() const override {
        return CMD_TMR;
    }
    
    virtual std::string desc() const override {
        return DESC_TMR;
    }
    
    virtual std::string arg() const override {
        return fmt("X=%s, NN=%s", reg(inst >> 8), hex(inst & 0xFF, 2));
    }

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

class UnknownInst: public InstTrait<UnknownInst> {
public:
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

    virtual void execute(Chip8& chip8, std::vector<uint32_t>& frame_buffer, int frame_width, int frame_height);
};

#endif