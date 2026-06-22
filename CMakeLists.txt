cmake_minimum_required(VERSION 3.16)
project(gameboy CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gameboy
    src/main.cpp
    src/gb.cpp
    src/cpu.cpp
    src/bus.cpp
    src/cartridges/cartridge.cpp
)

target_include_directories(gameboy PRIVATE src)

target_compile_options(gameboy PRIVATE -Wall -Wextra)
