cmake_minimum_required(VERSION 3.16)
project(gameboy CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(gameboy
    src/main.cpp
    src/gb.cpp
    src/cpu.cpp
    src/cpu_cb.cpp
    src/bus.cpp
    src/ppu.cpp
    src/display.cpp
    src/cartridges/cartridge.cpp
    src/cartridges/no_mbc.cpp
)

target_include_directories(gameboy PRIVATE src)
target_link_libraries(gameboy PRIVATE SDL2::SDL2)
target_compile_options(gameboy PRIVATE -Wall -Wextra)
