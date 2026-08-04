#include "ppu.h"
#include "bus.h"
#include <cstdint>


PPU::PPU(Bus& bus) : bus_(bus) {

}


void PPU::tick(int cycles) {
    current_cycles_ += cycles;
    while (current_cycles_ >= 456) {
        current_cycles_ -= 456;
        current_scanline_ ++;
        bus_.write8(0xFF44, current_scanline_);

        if (current_scanline_ < 144) {
            render_scanline();
        }

        if (current_scanline_ == 144) {
            // Entered Vertical Blank, so set bottom bit on IF reg to on.
            uint8_t if_register = bus_.read8(0xFF0F);
            if_register |= 0x01;
            bus_.write8(0xFF0F, if_register);
        }

        if (current_scanline_ > 153) {
            current_scanline_ = 0;
        }
    }
}


uint8_t PPU::read_register(uint16_t address) const {
    return 0xFF;
}


void PPU::write_register(uint16_t address, uint8_t value) {

}


uint8_t PPU::decode_pixel(uint8_t low_byte, uint8_t high_byte, int pos) const {
    int pixel = 7 - pos;
    int low = (low_byte >> pixel) & 1;
    int high = (high_byte >> pixel) & 1;
    uint8_t shade = (high << 1) | low;
    return shade;
}


void PPU::render_scanline() {
    int y = current_scanline_;

    for (int x = 0; x < 160; x++) {
        // Figuring out which 8x8 tile this pixel falls into
        int tile_col = x / 8;
        int tile_row = y / 8;

        uint16_t map_address = 0x9800 + (tile_row * 32) + tile_col;
        uint8_t tile = bus_.read8(map_address);

        // Specific pixel within the tile
        int pixel_col = x % 8;
        int pixel_row = y % 8;

        uint16_t tile_address = 0x8000 + (tile * 16) + (pixel_row * 2);
        uint8_t low_byte = bus_.read8(tile_address);
        uint8_t high_byte = bus_.read8(tile_address + 1);

        uint8_t shade = decode_pixel(low_byte, high_byte, pixel_col);

        frame_[y * 160 + x] = shade;
    }
}
