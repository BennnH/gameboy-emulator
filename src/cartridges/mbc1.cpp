#include "mbc1.h"
#include <cstdint>

MBC1::MBC1(std::vector<uint8_t> rom, int ram_size)
    : rom_(std::move(rom)), ram_(ram_size, 0){
}

uint8_t MBC1::read_rom(uint16_t address) const {
    // Lower consistent window
    if (address< 0x4000) {

        // There's a hardware quirk here that needs implementing on cartridges over 1MB.
        // Come back to do this later.
        return rom_[address];
    }

    // Upper window, this is the switchable bank. (0x4000 - 0x7FFF)
    uint8_t bank = (bank_hi_ << 5) | bank_lo_;
    uint32_t offset = (bank * 0x4000) + (address - 0x4000);
    return rom_[offset % rom_.size()];
}

void MBC1::write_rom(uint16_t address, uint8_t value) {
    // Write to the lower, consistent window.
    if (address < 0x2000) {
        // Lower nibble = 0x0A means enable.
        ram_enabled_ = (value & 0x0F) == 0x0A;
    } else if (address < 0x4000){
        uint8_t bank = value & 0x1F;
        // MBC1 quirk -  a written value of 0 becomes 1 for this register in real hardware.
        if (bank == 0x00) {
            bank = 0x01;
        }
        bank_lo_ = bank;
    } else if (address < 0x6000) {
        bank_hi_ = value & 0x03;
    } else if (address < 0x8000) {
        // Banking mode select, 0 = ROM banking, 1 = RAM banking
        mode_ = value & 0x01;
    }
}


 uint8_t MBC1::read_ram(uint16_t address) const {
     if (!ram_enabled_ || ram_.empty()) {
         return  0xFF;
     }

     // RAM bank only applies in mode 1; in mode 0 it's always bank 0.
     uint8_t bank = mode_ ? bank_hi_ : 0;
     uint32_t offset = (bank * 0x2000) + (address - 0xA000);
     return ram_[offset % ram_.size()];
 }


 void MBC1::write_ram(uint16_t address, uint8_t value) {
     if (!ram_enabled_ || ram_.empty()) {
         return;
     }

     uint8_t bank = mode_ ? bank_hi_ : 0;
     uint32_t offset = (bank * 0x2000) + (address - 0xA000);
     ram_[offset % ram_.size()] = value;
 }


 const std::vector<uint8_t>& MBC1::get_ram() const {
     return ram_;
 }


 void MBC1::load_ram(const std::vector<uint8_t>& data) {
     if (data.size() == ram_.size()) {
         ram_ = data;
     }
 }
