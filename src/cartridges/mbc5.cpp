#include "mbc5.h"


MBC5::MBC5(std::vector<uint8_t> rom, int ram_size)
    : rom_(std::move(rom)), ram_(ram_size, 0) {
}


uint8_t MBC5::read_rom(uint16_t address) const {
    if (address < 0x4000) {
        // Lower window: always bank 0. Read straight from the start of ROM.
        return rom_[address];
    }

    // Upper window (0x4000-0x7FFF) is the switchable bank.
    // Working out the real offset into the full ROM for the current memory bank.
    uint32_t offset = (rom_bank_ * 0x4000) + (address - 0x4000);
    return rom_[offset];
}


void MBC5::write_rom(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        // 0x0000-0x1FFF: RAM enable. Low nibble == 0xA enables
        ram_enabled_ = (value & 0x0F) == 0x0A;
    } else if (address < 0x3000) {
        // 0x2000-0x2FFF: low 8 bits of the ROM bank number.
        rom_bank_ = (rom_bank_ & 0x100) | value;
    } else if (address < 0x4000) {
        // 0x3000-0x3FFF: 9th bit of the ROM bank number.
        rom_bank_ = (rom_bank_ & 0x0FF) | ((value & 0x01) << 8);
    } else if (address < 0x6000) {
        // 0x4000-0x5FFF: RAM bank number (low 4 bits).
        ram_bank_ = value & 0x0F;
    }
    // 0x6000-0x7FFF: nothing on MBC5 (this range does something on MBC1, not here).
}


uint8_t MBC5::read_ram(uint16_t address) const {
    if (!ram_enabled_ || ram_.empty()) {
        // RAM locked or cartridge has none so bus back open
        return 0xFF;
    }

    uint32_t offset = (ram_bank_ * 0x2000) + (address - 0xA000);
    return ram_[offset];
}


void MBC5::write_ram(uint16_t address, uint8_t value) {
    if (!ram_enabled_ || ram_.empty()) {
        // RAM locked or cartridge has none so writes are dropped.
        return;
    }

    uint32_t offset = (ram_bank_ * 0x2000) + (address - 0xA000);
    ram_[offset] = value;
}
