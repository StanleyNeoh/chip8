#ifndef SRC_LIB_INSTRUCTIONS_INSTRUCTION_GRP_HPP
#define SRC_LIB_INSTRUCTIONS_INSTRUCTION_GRP_HPP

#include <string>
#include <memory>
#include <cstdint>
#include "../utils/format.hpp"
#include "instructions.hpp"

class InstGrp {
protected:
    inst_t inst;
    InstGrp(inst_t inst): inst(inst) {}
public:
    std::unique_ptr<Inst> parse();
};

class MachineInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x0000;
    MachineInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        switch (inst & 0x0FFF) {
            case ClearScreen::op: return std::unique_ptr<Inst>(new ClearScreen(inst));
            case ReturnInst::op:  return std::unique_ptr<Inst>(new ReturnInst(inst));
            default: return nullptr;
        }
    }
};

class JumpInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x1000;
    JumpInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new JumpInst(inst));
    }
};

class SubroutInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x2000; 
    SubroutInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SubroutInst(inst));
    }
};

class SkipConstEqInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x3000;
    SkipConstEqInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SkipConstEqInst(inst));
    }
};

class SkipConstNeqInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x4000;
    SkipConstNeqInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SkipConstNeqInst(inst));
    }
};

class SkipRegEqInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x5000;
    SkipRegEqInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SkipRegEqInst(inst));
    }
};

class SkipRegNeqInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x9000;
    SkipRegNeqInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SkipRegNeqInst(inst));
    }
};

class SetConstInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x6000;
    SetConstInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SetConstInst(inst));
    }
};

class AddConstInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x7000;
    AddConstInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new AddConstInst(inst));
    }
};

class ArithInstGrp: public InstGrp {
public:
    static const inst_t grp = 0x8000;
    ArithInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        switch (inst & 0x000F) {
            case LoadReg::op:
                return std::unique_ptr<Inst>(new LoadReg(inst));
            case OrReg::op:
                return std::unique_ptr<Inst>(new OrReg(inst));
            case AndReg::op:
                return std::unique_ptr<Inst>(new AndReg(inst));
            case XorReg::op:
                return std::unique_ptr<Inst>(new XorReg(inst));
            case AddReg::op:
                return std::unique_ptr<Inst>(new AddReg(inst));
            case SubReg::op:
                return std::unique_ptr<Inst>(new SubReg(inst));
            case ShiftRightInst::op:
                return std::unique_ptr<Inst>(new ShiftRightInst(inst));
            case SubNReg::op:
                return std::unique_ptr<Inst>(new SubNReg(inst));
            case ShiftLeftInst::op:
                return std::unique_ptr<Inst>(new ShiftLeftInst(inst));
        }
        return nullptr;
    }
};

class SetIndexInstGrp: public InstGrp {
public:
    static const inst_t grp = 0xA000;
    SetIndexInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SetIndexInst(inst));
    }
};


class JumpOffsetInstGrp: public InstGrp {
public:
    static const inst_t grp = 0xB000;
    JumpOffsetInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new JumpOffsetInst(inst));
    }
};


class RandInstGrp: public InstGrp {
public:
    static const inst_t grp = 0xC000;
    RandInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new RandInst(inst));
    }
};


class DisplayInstGrp: public InstGrp {
public:
    static const inst_t grp = 0xD000;
    DisplayInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new DisplayInst(inst));
    }
};


class SkipIfKeyInstGrp: public InstGrp {
public:
    static const inst_t grp = 0xE000;
    SkipIfKeyInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new SkipIfKeyInst(inst));
    }
};


class TimerInstGrp: public InstGrp {
public:
    static const inst_t grp = 0xF000;
    TimerInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new TimerInst(inst));
    }
};


class UnknownInstGrp: public InstGrp {
public:
    UnknownInstGrp(inst_t inst): InstGrp(inst) {}
    std::unique_ptr<Inst> parse() {
        return std::unique_ptr<Inst>(new UnknownInst(inst));
    }
};

#endif