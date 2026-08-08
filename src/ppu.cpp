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
            current_window_line_ = 0;
            bus_.write8(0xFF44, 0);
            uint8_t stat_reg = bus_.read8(0xFF41);
            bus_.write8(0xFF41, stat_reg & 0xFC);
        }
        return;
    }

    current_cycles_ += cycles;
    while (current_cycles_ >= 456) {
        current_cycles_ -= 456;

        if (current_scanline_ < 144) {
            render_scanline();
            render_window();
            render_sprites();
        }

        current_scanline_ ++;
        bus_.write8(0xFF44, current_scanline_);

        if (current_scanline_ == 144) {
            // Entered Vertical Blank, so set bottom bit on IF reg to on.
            uint8_t if_register = bus_.read8(0xFF0F);
            if_register |= 0x01;
            bus_.write8(0xFF0F, if_register);
        }

        if (current_scanline_ > 153) {
            current_scanline_ = 0;
            current_window_line_ = 0;
            bus_.write8(0xFF44, current_scanline_);
        }
    }

    uint8_t mode;
    if (current_scanline_ >= 144) {
        mode = 1;
    } else if (current_cycles_ < 80) {
        mode = 2;
    } else if (current_cycles_ < 252) {
        mode = 3;
    } else {
        mode = 0;
    }

    uint8_t stat_reg = bus_.read8(0xFF41);
    stat_reg = (stat_reg & 0xFC) | mode;

    uint8_t lyc = bus_.read8(0xFF45);
    if (current_scanline_ == lyc) {
        stat_reg |= 0x04;
    } else {
        stat_reg &= ~0x04;
    }
    bus_.write8(0xFF41, stat_reg);
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
    uint16_t map_base = (LCDC_reg & 0x08) ? 0x9C00 : 0x9800;

    if (!(LCDC_reg & 0x01)) {
        // Bit 0 off: background disabled, this line shows blank (shade 0)
        for (int x = 0; x < 160; x++) {
            bg_colour_index_[y * 160 + x] = 0;
            frame_[y * 160 + x] = 0;
        }
        return;
    }

    for (int x = 0; x < 160; x++) {
        // Figuring out which 8x8 tile this pixel falls into, using the scroll offset + wrapping it.
        int background_x = (x + scx_reg) % 256;
        int background_y = (y + scy_reg) % 256;

        int tile_col = background_x / 8;
        int tile_row = background_y / 8;

        uint16_t map_address = map_base + (tile_row * 32) + tile_col;
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

        bg_colour_index_[y * 160 + x] = colour_index;
        frame_[y * 160 + x] = shade;
    }
}


void PPU::render_window() {
    uint8_t LCDC_reg = bus_.read8(0xFF40);

    // Bit 5 = window enable. Bit 0 = disable both bg + window.
    if (!(LCDC_reg & 0x20) || !(LCDC_reg & 0x01)) {
        return;
    }

    uint8_t wy = bus_.read8(0xFF4A);
    int y = current_scanline_;

    // Window hasn't reached its starting row this frame so nothing to draw
    if (y < wy) {
        return;
    }

    uint8_t wx = bus_.read8(0xFF4B);
    int window_x_start = wx - 7;

    // Bit 6 = window's own tilemap select
    uint16_t map_base = (LCDC_reg & 0x40) ? 0x9C00 : 0x9800;

    bool unsigned_mode = LCDC_reg & 0x10;

    for (int x = 0; x < 160; x++) {
        int screen_x = x - window_x_start;

        // This pixel hasn't reached the window's left edge yet - skip it,
        // background stays showing here.
        if (screen_x < 0) {
            continue;
        }

        int tile_col = screen_x / 8;
        int tile_row = current_window_line_ / 8;

        uint16_t map_address = map_base + (tile_row * 32) + tile_col;
        uint8_t tile = bus_.read8(map_address);

        int pixel_col = screen_x % 8;
        int pixel_row = current_window_line_ % 8;

        uint16_t tile_address;
        if (unsigned_mode) {
            tile_address = 0x8000 + (tile * 16) + (pixel_row * 2);
        } else {
            int8_t signed_tile = (int8_t)tile;
            tile_address = 0x9000 + (signed_tile * 16) + (pixel_row * 2);
        }

        uint8_t low_byte = bus_.read8(tile_address);
        uint8_t high_byte = bus_.read8(tile_address + 1);

        uint8_t colour_index = decode_pixel(low_byte, high_byte, pixel_col);

        uint8_t bgp = bus_.read8(0xFF47);
        uint8_t shade = (bgp >> (colour_index * 2)) & 0x03;

        bg_colour_index_[y * 160 + x] = colour_index;
        frame_[y * 160 + x] = shade;
    }

    current_window_line_++;
}


void PPU::render_sprites() {
    uint8_t LCDC_reg = bus_.read8(0xFF40);
    if (!(LCDC_reg & 0x02)) {
        return;
    }

    // LCDC reg can specift to have taller 8x16 sprites instead of the usual 8x8
    bool tall_sprites = LCDC_reg & 0x04;
    int sprite_height = tall_sprites ? 16 : 8;

    int y = current_scanline_;

    // Do a first pass. Real Hardware can only track 10 sprites per scanline, so scan first 10 that encounter.
    struct Candidate {
        Sprite sprite;
        int oam_index;
    };
    Candidate candidates[10];
    int candidate_count = 0;

    for (int i = 0; i < 40 && candidate_count < 10; i++) {
        Sprite current_sprite = get_sprite(i);
        int screen_y = current_sprite.y - 16;
        if (y >= screen_y && y < screen_y + sprite_height) {
            candidates[candidate_count] = { current_sprite, i };
            candidate_count++;
        }
    }

    // Need to order the sprites. Priority goes to lower x value, then ties are broken by lower OAM index.
    // So we need to draw lowest priority to highest priority, so the lower ones may be overwritten and the higher
    // priority ends up being drawn on top. Just a bubble sort with tie breaks.
    for (int i = 0; i < candidate_count; i++) {
        for (int j = i + 1; j < candidate_count; j++) {
            bool should_swap = candidates[j].sprite.x > candidates[i].sprite.x ||
                (candidates[j].sprite.x == candidates[i].sprite.x &&
                 candidates[j].oam_index > candidates[i].oam_index);
            if (should_swap) {
                Candidate temp = candidates[i];
                candidates[i] = candidates[j];
                candidates[j] = temp;
            }
        }
    }

    // Draw in reverse priority order so sprites with lowest priority get drawn first.

    for (int i = 0; i < candidate_count; i++) {
        Sprite current_sprite = candidates[i].sprite;

        int screen_y = current_sprite.y - 16;
        int screen_x = current_sprite.x - 8;

        if (y >= screen_y && y < screen_y + sprite_height) {
            int row = y - screen_y;
            bool y_flip = current_sprite.flags & 0x40;
            int effective_row = y_flip ? (sprite_height - 1 - row) : row;

            uint8_t tile_index;
            int row_within_tile;
            if (tall_sprites) {
                uint8_t base_tile = current_sprite.tile & 0xFE;
                if (effective_row < 8) {
                    tile_index = base_tile;
                    row_within_tile = effective_row;
                } else {
                    tile_index = base_tile + 1;
                    row_within_tile = effective_row - 8;
                }
            } else {
                tile_index = current_sprite.tile;
                row_within_tile = effective_row;
            }

            uint16_t tile_address = 0x8000 + (tile_index * 16) + (row_within_tile * 2);
            uint8_t low_byte  = bus_.read8(tile_address);
            uint8_t high_byte = bus_.read8(tile_address + 1);

            // These are behaviors defined in the sprites flag.
            // Bit 4 of the sprite's flags picks which palette: 0 = OBP0, 1 = OBP1
            // bg_priority is for times when we want to break the regular rule of sprites having priority over background
            // and the background is rendered over the sprite. Like if we wanted a sprite to go behind a tree or a door.
            uint16_t palette_address = (current_sprite.flags & 0x10) ? 0xFF49 : 0xFF48;
            uint8_t obp = bus_.read8(palette_address);
            bool x_flip = current_sprite.flags & 0x20;
            bool bg_priority = current_sprite.flags & 0x80;

            for (int col = 0; col < 8; col++) {
                // Accounts for if the flag for the sprite says to flip the sprite or not.
                int effective_col = x_flip ? (7 - col) : col;
                uint8_t colour = decode_pixel(low_byte, high_byte, effective_col);

                // Skip if colour is 0 (transparent)
                if (colour == 0) {
                    continue;
                }

                int pixel_x = screen_x + col;
                // Skip if the sprite goes out of bounds.
                if (pixel_x < 0 || pixel_x >= 160) {
                    continue;
                }

                if (bg_priority && bg_colour_index_[y * 160 + pixel_x] != 0) {
                    continue;
                }

                uint8_t shade = (obp >> (colour * 2)) & 0x03;
                frame_[y * 160 + pixel_x] = shade;
            }
        }
    }
}


Sprite PPU::get_sprite(int index) const {
    uint16_t address = 0xFE00 + (index * 4);
    uint8_t y = bus_.read8(address);
    uint8_t x = bus_.read8(address + 1);
    uint8_t tile = bus_.read8(address + 2);
    uint8_t flags = bus_.read8(address + 3);

    Sprite current_sprite = { y, x, tile, flags };
    return current_sprite;
}
