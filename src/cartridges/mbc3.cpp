#include "mbc3.h"
#include <cstdint>

MBC3::MBC3(std::vector<uint8_t> rom, int ram_size)
    : rom_(std::move(rom)), ram_(ram_size, 0) {
}

uint8_t MBC3::read_rom(uint16_t address) const {
    if (address < 0x4000) {
        // Lower window always bank 0
        return rom_[address];
    }

    // Upper window (0x4000-0x7FFF) - switchable bank.
    uint32_t offset = (rom_bank_ * 0x4000) + (address - 0x4000);
    return rom_[offset % rom_.size()];
}

void MBC3::write_rom(uint16_t address, uint8_t value) {
    // Write to the lower, consistent window.
    if (address < 0x2000) {
        // Lower nibble = 0x0A means enable.
        // This would also enable the RTC when implemented.
        ram_enabled_ = (value & 0x0F) == 0x0A;
    } else if (address < 0x4000){
        uint8_t bank = value & 0x7F;
        // MBC3 quirk -  a written value of 0 becomes 1 for this register in real hardware.
        if (bank == 0x00) {
            bank = 0x01;
        }
        rom_bank_ = bank;
    } else if (address < 0x6000) {
        // 0x4000-0x5FFF: RAM bank select (0x00-0x03), OR RTC register select
        // (0x08-0x0C). RTC not implemented yet

        ram_bank_ = value;

    } else if (address < 0x8000) {
        // 0x6000-0x7FFF: RTC latch (write 0 then 1 to latch clock into registers).
        // TODO: implement RTC latching.
    }
}


uint8_t MBC3::read_ram(uint16_t address) const {
    if (!ram_enabled_ || ram_.empty()) {
        return 0xFF;
    }

    // ram_bank_ 0x00-0x03 selects a RAM bank. Values 0x08-0x0C would select an
    // RTC register instead not implemented yet, so those reads just go to
    // RAM indexes for now
    uint32_t offset = (ram_bank_ * 0x2000) + (address - 0xA000);
    return ram_[offset % ram_.size()];
}

void MBC3::write_ram(uint16_t address, uint8_t value) {
    if (!ram_enabled_ || ram_.empty()) {
        return;
    }

    uint32_t offset = (ram_bank_ * 0x2000) + (address - 0xA000);
    ram_[offset % ram_.size()] = value;
}


const std::vector<uint8_t>& MBC3::get_ram() const {
    return ram_;
}


void MBC3::load_ram(const std::vector<uint8_t>& data) {
    if (data.size() == ram_.size()) {
        ram_ = data;
    }
}
