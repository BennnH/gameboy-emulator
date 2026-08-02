#include "ppu.h"
#include "bus.h"
#include <cstdint>
#include <cstdio>


PPU::PPU(Bus& bus) : bus_(bus) {

}


void PPU::tick(int cycles) {
    current_cycles_ += cycles;
    while (current_cycles_ >= 456) {
        current_cycles_ -= 456;
        current_scanline_ ++;



        if (current_scanline_ == 144) {
            // Entered Vertical Blank, so set bottom bit on IF reg to on.
            uint8_t if_register = bus_.read8(0xFF0F);
            if_register |= 0x01;
            bus_.write8(0xFF0F, if_register);
            printf("VBlank\n");
        }

        if (current_scanline_ > 153) {
            current_scanline_ = 0;
        }
        //printf("Current Scanline: %d\n", current_scanline_);
    }
}

uint8_t PPU::read_register(uint16_t address) const {
    return 0xFF;
}

void PPU::write_register(uint16_t address, uint8_t value) {

}
