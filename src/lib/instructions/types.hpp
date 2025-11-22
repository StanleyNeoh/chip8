#ifndef SRC_LIB_INSTRUCTIONS_TYPES_HPP
#define SRC_LIB_INSTRUCTIONS_TYPES_HPP

#include <cstdint>

using inst_t = uint16_t;

// Instruction mnemonics
#define CMD_CLS "CLS"
#define CMD_RET "RET"
#define CMD_JP "JP"
#define CMD_CALL "CALL"
#define CMD_SE "SE"
#define CMD_SNE "SNE"
#define CMD_SEV "SEV"
#define CMD_SNEV "SNEV"
#define CMD_LDV "LDV"
#define CMD_ADDV "ADDV"
#define CMD_LDR "LDR"
#define CMD_OR "OR"
#define CMD_AND "AND"
#define CMD_XOR "XOR"
#define CMD_ADDR "ADDR"
#define CMD_SUB "SUB"
#define CMD_SHR "SHR"
#define CMD_SUBN "SUBN"
#define CMD_SHL "SHL"
#define CMD_LDI "LDI"
#define CMD_JP0 "JP0"
#define CMD_RND "RND"
#define CMD_DRW "DRW"
#define CMD_SKP "SKP"
#define CMD_TMR "TMR"
#define CMD_UNK "UNK"

// Instruction descriptions
#define DESC_CLS "Clear the display"
#define DESC_RET "Return from subroutine"
#define DESC_JP "Jump to address"
#define DESC_CALL "Call subroutine"
#define DESC_SE "Skip next if Vx == kk"
#define DESC_SNE "Skip next if Vx != kk"
#define DESC_SEV "Skip next if Vx == Vy"
#define DESC_SNEV "Skip next if Vx != Vy"
#define DESC_LDV "Set Vx = kk"
#define DESC_ADDV "Add kk to Vx"
#define DESC_LDR "Set Vx = Vy"
#define DESC_OR "Vx = Vx OR Vy"
#define DESC_AND "Vx = Vx AND Vy"
#define DESC_XOR "Vx = Vx XOR Vy"
#define DESC_ADDR "Vx += Vy; VF carry"
#define DESC_SUB "Vx = Vx - Vy; VF borrow"
#define DESC_SHR "Vx >>= 1; VF = LSB"
#define DESC_SUBN "Vx = Vy - Vx; VF borrow"
#define DESC_SHL "Vx <<= 1; VF = MSB"
#define DESC_LDI "I = nnn"
#define DESC_JP0 "Jump to nnn + V0"
#define DESC_RND "Vx = rand & kk"
#define DESC_DRW "Draw sprite at (Vx,Vy) height n"
#define DESC_SKP "Skip next if key Vx pressed"
#define DESC_TMR "Timer / memory group"
#define DESC_UNK "Unknown instruction"

#endif