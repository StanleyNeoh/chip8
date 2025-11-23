cmake_minimum_required(VERSION 3.16)
project(chip8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL) # Process Cmake in subdirectory
add_subdirectory(src/lib) # Build the chip8 library

include_directories(src/lib)
link_libraries(chip8lib)

# Executable targets
add_executable(chip8 src/main.cpp)
add_executable(decompile src/decompile.cpp)