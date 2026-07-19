#include "bus.h"
#include "cartridges/cartridge.h"

#include <cstdio>


Bus::Bus(Cartridge& cartridge) : cartridge_(cartridge) {
}

uint8_t Bus::read8(uint16_t address) const {
    // Memory region for rom/cartridge (0x0000 - 0x7FFF)
    if (address <= 0x7FFF) {
        return cartridge_.read(address);
    }
    // For now everything else is just this temp memory array for now
    return this->memory_[address];
}

void Bus::write8(uint16_t address, uint8_t value) {
    // For now roms are read only, and we don't write to cartridges so ignore.
    if (address <= 0x7FFF) {
        return;
    }
    this->memory_[address] = value;

    if (address == 0xFF02 && value == 0x81) {
        std::printf("%c", this->memory_[0xFF01]);
        // Force output the contents, wouldn't happen without this in an inf loop
        std::fflush(stdout);
        // Clear to signal the transfer is done which is done by actual hardware.
        this->memory_[0xFF02] = 0x01;
    }
}
