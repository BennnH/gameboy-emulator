#include "gb.h"

Gameboy::Gameboy() : bus_(cartridge_), cpu_(bus_), ppu_(bus_){
    cpu_.reset();
}

bool Gameboy::load_rom(const std::string& filename) {
    return cartridge_.load(filename);
}

void Gameboy::step() {
    int cycles = cpu_.step();
    bus_.tick(cycles);
    ppu_.tick(cycles);
}

void Gameboy::run(int max_steps) {
    for (int i = 0; i < max_steps; i ++) {
        step();
    }
}
