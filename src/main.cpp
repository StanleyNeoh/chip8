#include <iostream>
#include <memory>
#include <cstring>
#include <fstream>
#include "lib/chip8/chip8.hpp"
#include "lib/ui/ui.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2 || !argv[1]) {
        std::cerr << "Usage: " << argv[0] << " <rom_file>\n";
        return 1;
    }
    const char* rom_path = argv[1];

    Chip8 chip8;
    if (!chip8.loadRom(rom_path)) {
        throw std::runtime_error("Failed to load ROM");
        return 1;
    }

    UI& ui = UI::create("Chip8", 64, 32, chip8);
    ui.run();
    return 0;
}