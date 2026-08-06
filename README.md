# Game Boy Emulator

A Game Boy (DMG) emulator written from scratch in C++. The CPU is complete and passes Blargg's `cpu_instrs` and `instr_timing` tests. SDL 2 display also implemented rendering background and sprites, and takes user input interacting with ROMs.

## Build

```
cmake -B build
cmake --build build
./build/gameboy <rom>

```

## TODO

1. Add debug tool (Step through instructions, show registers, etc.)
2. Add MBC support
3. Add APU
4. Build GUI frontend
