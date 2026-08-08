#pragma once

#include <cstdint>


class MBC {
    public:
        virtual ~MBC() = default;

        // Writes to the ROM region (0x0000-0x7FFF)
        virtual uint8_t read_rom(uint16_t address) const = 0;
        virtual void write_rom(uint16_t address, uint8_t value) = 0;

        // Cartridge RAM region (0xA000-0xBFFF)
        virtual uint8_t read_ram(uint16_t address) const = 0;
        virtual void write_ram(uint16_t address, uint8_t value) = 0;



};
