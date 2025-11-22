#ifndef SRC_LIB_INSTRUCTIONS_PARSER_HPP
#define SRC_LIB_INSTRUCTIONS_PARSER_HPP

#include <memory>
#include "instruction_grp.hpp"

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
        switch (op & 0xF000) {
            case MachineInstGrp::grp:     return MachineInstGrp(op).parse();
            case JumpInstGrp::grp:        return JumpInstGrp(op).parse();
            case SubroutInstGrp::grp:     return SubroutInstGrp(op).parse();
            case SkipConstEqInstGrp::grp: return SkipConstEqInstGrp(op).parse();
            case SkipConstNeqInstGrp::grp:return SkipConstNeqInstGrp(op).parse();
            case SkipRegEqInstGrp::grp:   return SkipRegEqInstGrp(op).parse();
            case SkipRegNeqInstGrp::grp:  return SkipRegNeqInstGrp(op).parse();
            case SetConstInstGrp::grp:    return SetConstInstGrp(op).parse();
            case AddConstInstGrp::grp:    return AddConstInstGrp(op).parse();
            case ArithInstGrp::grp:       return ArithInstGrp(op).parse();
            case SetIndexInstGrp::grp:    return SetIndexInstGrp(op).parse();
            case JumpOffsetInstGrp::grp:  return JumpOffsetInstGrp(op).parse();
            case RandInstGrp::grp:        return RandInstGrp(op).parse();
            case DisplayInstGrp::grp:     return DisplayInstGrp(op).parse();
            case SkipIfKeyInstGrp::grp:   return SkipIfKeyInstGrp(op).parse();
            case TimerInstGrp::grp:       return TimerInstGrp(op).parse();
            default:                     return UnknownInstGrp(op).parse();
        }
    }

    const std::vector<std::unique_ptr<Inst>>& getInstructions() const {
        return parsed_insts;
    }

    friend std::ostream& operator<<(std::ostream&, const Chip8Parser&);
};

std::ostream& operator<<(std::ostream& out, const Chip8Parser& parser) {
    out << "=== " << parser.filename << " ===\n";
    for (const auto& inst_ptr: parser.getInstructions()) {
        out << std::hex << (inst_ptr->inst) << ": "
            << inst_ptr->cmd() << " "
            << inst_ptr->arg() << " -- "
            << inst_ptr->desc() << "\n";
    }
    out << "=== END ===";
    return out;
}

#endif