#include "gb.h"
#include "cpu.h"

Gameboy::Gameboy() : bus_(cartridge_), cpu_(bus_) {
    cpu_.reset();
}

bool Gameboy::load_rom(const std::string& filename) {
    return cartridge_.load(filename);
}

void Gameboy::step() {
    cpu_.step();
}

void Gameboy::run(int max_steps) {
    for (int i = 0; i < max_steps; i ++) {
        cpu_.step();
    }
}
