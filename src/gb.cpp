#include "gb.h"

Gameboy::Gameboy() : bus_(cartridge_), cpu_(bus_), ppu_(bus_){
    bus_.reset();
    cpu_.reset();
}

bool Gameboy::load_rom(const std::string& filename) {
    return cartridge_.load(filename);
}

int Gameboy::step() {
    int cycles = cpu_.step();
    bus_.tick(cycles);
    ppu_.tick(cycles);
    return cycles;
}


void Gameboy::run_frame() {
    int frame_cycles = 0;
    while (frame_cycles < 70224) {
        frame_cycles += step();
    }
}
