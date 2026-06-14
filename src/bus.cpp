#include "bus.h"

uint8_t Bus::read8(uint16_t address) const {
    return this->memory[address];
}

void Bus::write8(uint16_t address, uint8_t value) {
    this->memory[address] = value;
}
