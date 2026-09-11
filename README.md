# Game Boy Emulator

A Game Boy (DMG) emulator written in C++23, using SDL2 for the display and audio.

## Features

**CPU:** Full Sharp SM83 instruction set including the CB-prefixed table, with accurate instruction timings, all five interrupts, the `EI` delay quirk, and the `HALT` bug.
 
**PPU:** Scanline-based background, window and sprite rendering, including the 10-sprites-per-scanline hardware limit and sprite priority by X coordinate with OAM index tie-breaking. STAT interrupts are edge-triggered on the internal STAT line, so they fire once per transition rather than continuously while a condition holds.
 
**APU:** All four audio channels: two pulse channels with selectable duty cycles, a wave channel playing 32 user-defined samples from wave RAM, and a noise channel driven by a 15-bit LFSR. Includes volume envelopes, length counters, channel 1's frequency sweep, and the NR50/NR51 stereo mixing and routing matrix. Output is downsampled from the 4.19 MHz hardware clock to 44.1 kHz.
 
**Cartridges:** No-MBC, MBC1, MBC2, MBC3 and MBC5, covering the large majority of the commercially available ROMs. Battery-backed cartridges save to a `.sav` file alongside the ROM, in the flat SRAM-dump format consistent with other emulators.
 
**Timing:** Frame pacing tracks the true DMG refresh rate of 59.7275 Hz using a fractional accumulator, rather than rounding to whole milliseconds.

## Build

Requires CMake 3.16+, a C++23 compiler, and SDL2.

```
cmake -B build
cmake --build build
./build/gameboy <rom_path>

```
## Controls

| Game Boy | Keyboard    |
|----------|-------------|
| D-Pad    | Arrow Keys  |
| A        | Z           |
| B        | X           |
| Start    | Enter       |
| Select   | Right Shift |


## Accuracy

Passes the following Blargg test ROMs:
- `cpu_instrs` (all 11 individual sub-tests)
- `instr_timing`
- `halt_bug`
