#pragma once

#include <array>
#include <cstdint>

class Cartridge;

class Bus {
    public:
        explicit Bus(Cartridge& cartridge);

        uint8_t read8(uint16_t address) const;
        void write8(uint16_t address, uint8_t value);
    private:
        Cartridge& cartridge_;
        // Used for now for non rom regions.
        std::array<uint8_t, 0x10000> memory_{};

};
