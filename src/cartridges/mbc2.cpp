#include "mbc2.h"
#include <cstdint>

// MBC2 doesnt have variable ram, just 512 x half bytes build into the cartridge.
MBC2::MBC2(std::vector<uint8_t> rom, int )
    : rom_(std::move(rom)), ram_(512, 0) {
}


uint8_t MBC2::read_rom(uint16_t address) const {
    if (address < 0x4000) {
        // Lower window always bank 0
        return rom_[address];
    }

    // Upper window (0x4000-0x7FFF) - switchable bank.
    uint32_t offset = (rom_bank_ * 0x4000) + (address - 0x4000);
    return rom_[offset % rom_.size()];
}


void MBC2::write_rom(uint16_t address, uint8_t value) {
    if (address < 0x4000) {
        // MBC2 uses bit 8 of the address to pick which register. 0 = RAM enable. 1 = ROM bank select.
        if (address & 0x0100) {
            // ROM bank - low 4 bits, 0 becomes 1.
            uint8_t bank = value & 0x0F;
            if (bank == 0x00) {
                bank = 0x01;
            }
            rom_bank_ = bank;
        } else {
            // low nibble == 0xA enables RAM
            ram_enabled_ = (value & 0x0F) == 0x0A;
        }
    }
    // Writes at 0x4000-0x7FFF do nothing on MBC2.
}


uint8_t MBC2::read_ram(uint16_t address) const {
    if (!ram_enabled_) {
        return 0xFF;
    }

    uint16_t index = (address - 0xA000) & 0x01FF;
    return ram_[index] | 0xF0;
}


void MBC2::write_ram(uint16_t address, uint8_t value) {
    if (!ram_enabled_) {
        return;
    }

    uint16_t index = (address - 0xA000) & 0x01FF;
    ram_[index] = value & 0x0F;
}


const std::vector<uint8_t>& MBC2::get_ram() const {
    return ram_;
}


void MBC2::load_ram(const std::vector<uint8_t>& data) {
    if (data.size() == ram_.size()) {
        ram_ = data;
    }
}
