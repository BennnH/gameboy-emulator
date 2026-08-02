#pragma once

#include <array>
#include <cstdint>
class Bus;

class PPU {
    public:
        explicit PPU(Bus& bus);

        void tick(int cycles);
        uint8_t read_register(uint16_t address) const;
        void write_register(uint16_t address, uint8_t value);

    private:
        Bus& bus_;

        std::array<uint8_t, 160*144> frame_{};

        uint8_t mode_{};
        uint8_t current_scanline_{};
        int current_cycles_{};
};
