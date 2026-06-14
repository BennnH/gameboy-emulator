cmake_minimum_required(VERSION 3.16)
project(gameboy CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gameboy
    src/main.cpp
    src/gb.cpp
    src/cpu.cpp
)

target_include_directories(gameboy PRIVATE src)

target_compile_options(gameboy PRIVATE -Wall -Wextra)
