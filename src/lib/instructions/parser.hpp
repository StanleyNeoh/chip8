#ifndef SRC_LIB_INSTRUCTIONS_PARSER_HPP
#define SRC_LIB_INSTRUCTIONS_PARSER_HPP

#include <memory>
#include "instructions.hpp"

#include <ios>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

class Chip8Decompiler {
protected:
    const char* filename;
    std::vector<inst_t> insts;
public:
    Chip8Decompiler(const char* filename): filename(filename) {
        std::ifstream fstream(filename, std::ios::binary);
        if (!fstream.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
        char buffer[2];
        while (fstream.read(buffer, 2)) {
            u_int8_t hi = static_cast<u_int8_t>(buffer[0]);
            u_int8_t lo = static_cast<u_int8_t>(buffer[1]);
            u_int16_t inst = hi << 8 | lo;
            insts.push_back(inst);
        }
    }

    friend std::ostream& operator<<(std::ostream&, const Chip8Decompiler&);
};

std::ostream& operator<<(std::ostream& out, const Chip8Decompiler& decompiler) {
    out << "=== " << decompiler.filename << " ===\n";
    for (inst_t i: decompiler.insts) {
        out << std::hex << i << "\n";
    }
    out << "=== END ===";
    return out;
}

class Chip8Parser: public Chip8Decompiler {
protected:
    std::vector<std::unique_ptr<Inst>> parsed_insts;
public:
    Chip8Parser(const char* filename): Chip8Decompiler(filename) {
        for (inst_t i: insts) {
            parsed_insts.push_back(parse(i));
        }
    }

    static std::unique_ptr<Inst> parse(inst_t op) {
        if (ClearScreen::match(op)) return std::make_unique<ClearScreen>(op);
        else if (ReturnInst::match(op)) return std::make_unique<ReturnInst>(op);
        else if (JumpInst::match(op)) return std::make_unique<JumpInst>(op);
        else if (SubroutInst::match(op)) return std::make_unique<SubroutInst>(op);
        else if (SkipConstEqInst::match(op)) return std::make_unique<SkipConstEqInst>(op);
        else if (SkipConstNeqInst::match(op)) return std::make_unique<SkipConstNeqInst>(op);
        else if (SkipRegEqInst::match(op)) return std::make_unique<SkipRegEqInst>(op);
        else if (SkipRegNeqInst::match(op)) return std::make_unique<SkipRegNeqInst>(op);
        else if (SetConstInst::match(op)) return std::make_unique<SetConstInst>(op);
        else if (AddConstInst::match(op)) return std::make_unique<AddConstInst>(op);
        else if (LoadReg::match(op)) return std::make_unique<LoadReg>(op);
        else if (OrReg::match(op)) return std::make_unique<OrReg>(op);
        else if (AndReg::match(op)) return std::make_unique<AndReg>(op);
        else if (XorReg::match(op)) return std::make_unique<XorReg>(op);
        else if (AddReg::match(op)) return std::make_unique<AddReg>(op);
        else if (SubXY::match(op)) return std::make_unique<SubXY>(op);
        else if (SubYX::match(op)) return std::make_unique<SubYX>(op);
        else if (ShiftRightInst::match(op)) return std::make_unique<ShiftRightInst>(op);
        else if (ShiftLeftInst::match(op)) return std::make_unique<ShiftLeftInst>(op);
        else if (SetIndexInst::match(op)) return std::make_unique<SetIndexInst>(op);
        else if (JumpOffsetInst::match(op)) return std::make_unique<JumpOffsetInst>(op);
        else if (RandInst::match(op)) return std::make_unique<RandInst>(op);
        else if (DisplayInst::match(op)) return std::make_unique<DisplayInst>(op);
        else if (SkipIfKPInst::match(op)) return std::make_unique<SkipIfKPInst>(op);
        else if (SkipIfNotKPInst::match(op)) return std::make_unique<SkipIfNotKPInst>(op);
        else if (TimerSetVXInst::match(op)) return std::make_unique<TimerSetVXInst>(op);
        else if (TimerSetDelayInst::match(op)) return std::make_unique<TimerSetDelayInst>(op);
        else if (TimerSetSoundInst::match(op)) return std::make_unique<TimerSetSoundInst>(op);
        else if (AddIRegInst::match(op)) return std::make_unique<AddIRegInst>(op);
        else if (GetKeyInst::match(op)) return std::make_unique<GetKeyInst>(op);
        else if (FontCharInst::match(op)) return std::make_unique<FontCharInst>(op);
        else if (BinCodedDecConvInst::match(op)) return std::make_unique<BinCodedDecConvInst>(op);
        else if (StoreMemInst::match(op)) return std::make_unique<StoreMemInst>(op);
        else if (LoadMemInst::match(op)) return std::make_unique<LoadMemInst>(op);
        else if (UnknownInst::match(op)) return std::make_unique<UnknownInst>(op);
        else return std::make_unique<UnknownInst>(op);
    }

    const std::vector<std::unique_ptr<Inst>>& getInstructions() const {
        return parsed_insts;
    }

    friend std::ostream& operator<<(std::ostream&, const Chip8Parser&);
};

std::ostream& operator<<(std::ostream& out, const Chip8Parser& parser) {
    out << "=== " << parser.filename << " ===\n";
    const std::vector<std::unique_ptr<Inst>>& insts = parser.getInstructions();
    for (int i = 0; i < insts.size(); i++) {
        const std::unique_ptr<Inst>& inst_ptr = insts[i];
        out << fmt("[%s, addr: %s] ", i, hex(0x200 + i * 2, 4))
            << std::hex << (inst_ptr->inst) << ": "
            << inst_ptr->cmd() << " "
            << inst_ptr->arg() << " -- "
            << inst_ptr->desc() << "\n";
    }
    out << "=== END ===";
    return out;
}

#endif