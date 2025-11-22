#include <iostream>
#include "lib/instructions/parser.hpp"

int main(int argc, char* argv[]) {
    if (argc > 2 || argc < 1) {
        std::cerr << "Usage: " << argv[0] << " [filename]\n";
        return 1;
    }
    const char* filename = argv[1];
    Chip8Parser decompiler(filename);
    std::cout << decompiler << "\n";
}