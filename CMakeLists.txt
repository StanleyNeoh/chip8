cmake_minimum_required(VERSION 3.16)
project(chip8)

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

add_executable(chip8 src/main.cpp)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)

add_executable(sdl-hello src/tests/sdl-hello.c)
target_link_libraries(sdl-hello PRIVATE SDL3::SDL3)

add_executable(ui-test src/tests/ui.cpp)
target_link_libraries(ui-test PRIVATE SDL3::SDL3)