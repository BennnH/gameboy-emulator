#include "ppu.h"
#include "bus.h"
#include <cstdint>


PPU::PPU(Bus& bus) : bus_(bus) {

}


void PPU::tick(int cycles) {

}

uint8_t PPU::read_register(uint16_t address) const {
    return 0xFF;
}

void PPU::write_register(uint16_t address, uint8_t value) {

}
