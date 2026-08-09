# Game Boy Emulator

A Game Boy (DMG) emulator written from scratch in C++ using SDL2 for the display.

## Build

```
cmake -B build
cmake --build build
./build/gameboy <rom_path>

```

## Test ROMs

Passes the following Blargg test ROMs:
- `cpu_instrs` (all 11 individual sub-tests)
- `instr_timing`
- `halt_bug`


## Current State

Full support for standard 32KiB cartridges and MBC5 cartridges, including saving to a `.sav` file.


## TODO

1. Add debug tool (Step through instructions, show registers, etc.)
2. Add support for remaining MBC cartridges
3. Add APU
4. Build GUI frontend with emulator settings (e.g. key mappings or audio volume)
5. Improve hardware accuracy to pass more test ROMs (e.g. mooneye)


## Controls

| Game Boy | Keyboard    |
|----------|-------------|
| D-Pad    | Arrow Keys  |
| A        | Z           |
| B        | X           |
| Start    | Enter       |
| Select   | Right Shift |
