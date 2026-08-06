#include "ppu.h"
#include "bus.h"
#include <cstdint>


PPU::PPU(Bus& bus) : bus_(bus) {

}


void PPU::tick(int cycles) {
    // Real hardware fully stops the PPU while the LCD is off (LCDC bit 7 = 0).
    // no rendering, no VBlank interrupt, until the game turns it back on.
    uint8_t lcdc = bus_.read8(0xFF40);
    if (!(lcdc & 0x80)) {
        if (current_scanline_ != 0 || current_cycles_ != 0) {
            current_scanline_ = 0;
            current_cycles_ = 0;
            bus_.write8(0xFF44, 0);
            uint8_t stat_reg = bus_.read8(0xFF41);
            bus_.write8(0xFF41, stat_reg & 0xFC);
        }
        return;
    }


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
            bus_.write8(0xFF44, current_scanline_);
        }
    }
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

    // Scroll registers, basically how far down / along the background the top left of the screen is.
    uint8_t scy_reg = bus_.read8(0xFF42);
    uint8_t scx_reg = bus_.read8(0xFF43);

    // If bit 4 of LCDC register is on, we get the tile data offset from 0x8000, otherwise we start from 0x9000
    uint8_t LCDC_reg = bus_.read8(0xFF40);
    bool unsigned_mode = LCDC_reg & 0x10;

    for (int x = 0; x < 160; x++) {
        // Figuring out which 8x8 tile this pixel falls into, using the scroll offset + wrapping it.
        int background_x = (x + scx_reg) % 256;
        int background_y = (y + scy_reg) % 256;

        int tile_col = background_x / 8;
        int tile_row = background_y / 8;

        uint16_t map_address = 0x9800 + (tile_row * 32) + tile_col;
        uint8_t tile = bus_.read8(map_address);

        // Specific pixel within the tile
        int pixel_col = background_x % 8;
        int pixel_row = background_y % 8;

        uint16_t tile_address;
        if (unsigned_mode) {
            tile_address = 0x8000 + (tile * 16) + (pixel_row * 2);
        } else {
            // Signed tile, so we can have negatives and go back in mem
            int8_t signed_tile = (int8_t)tile;
            tile_address = 0x9000 + (signed_tile * 16) + (pixel_row * 2);
        }

        uint8_t low_byte = bus_.read8(tile_address);
        uint8_t high_byte = bus_.read8(tile_address + 1);

        uint8_t colour_index = decode_pixel(low_byte, high_byte, pixel_col);

        // Getting actual palette shades from the palette register
        uint8_t bgp = bus_.read8(0xFF47);
        uint8_t shade = (bgp >> (colour_index * 2)) & 0x03;

        frame_[y * 160 + x] = shade;
    }
}
