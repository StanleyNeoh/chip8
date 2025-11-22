cmake_minimum_required(VERSION 3.16)

# Chip8 library
add_library(chip8lib STATIC
    chip8/chip8.cpp
    chip8/chip8.hpp
    instructions/instructions.cpp
    instructions/instructions.hpp
    instructions/instruction_grp.hpp
    instructions/parser.hpp
    instructions/types.hpp
    ui/ui.hpp
    utils/format.hpp
)

# Link SDL3 to the library
target_link_libraries(chip8lib PUBLIC SDL3::SDL3)