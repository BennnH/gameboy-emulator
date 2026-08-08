#include "no_mbc.h"

NoMBC::NoMBC(std::vector<uint8_t> rom) : rom_(std::move(rom)) {
}

uint8_t NoMBC::read_rom(uint16_t address) const {
    return rom_[address];
}

void NoMBC::write_rom(uint16_t, uint8_t) {
    // ROM-only cart: writes to the ROM region do nothing.
}

uint8_t NoMBC::read_ram(uint16_t) const {
    // No cartridge RAM - open bus reads back 0xFF.
    return 0xFF;
}

void NoMBC::write_ram(uint16_t, uint8_t) {
    // No cartridge RAM - writes are ignored.
}
