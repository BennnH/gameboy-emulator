# Game Boy Emulator

A Game Boy (DMG) emulator written from scratch in C++. The CPU is complete and passes Blargg's `cpu_instrs` and `instr_timing` tests.

## Build

```
cmake -B build
cmake --build build
./build/gameboy <rom>

```

## TODO

1. Add PPU
2. Add display output
3. Add input
4. Add debug tool (Step through instructions, show registers, etc.)
5. Add MBC support
6. Add APU
7. Build GUI frontend
