#include "bus.h"
#include "cartridges/cartridge.h"


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
}
