#pragma once

#include <array>
#include <cstdint>
class Bus;

struct Sprite {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t flags;
};


class PPU {
    public:
        explicit PPU(Bus& bus);

        void tick(int cycles);

        const std::array<uint8_t, 160*144>& get_frame() const {
            return frame_;
        }

    private:
        Bus& bus_;

        std::array<uint8_t, 160*144> frame_{};
        std::array<uint8_t, 160*144> bg_colour_index_{};

        uint8_t current_scanline_{};
        int current_window_line_{};
        int current_cycles_{};

        uint8_t decode_pixel(uint8_t low_byte, uint8_t high_byte, int x) const;
        void render_scanline();
        void render_window();
        Sprite get_sprite(int index) const;
        void render_sprites();
};
