#pragma once

#include "mbc.h"
#include <vector>
#include <cstdint>

// ROM-only cartridges , only 32KiB with no memory banking.
class NoMBC : public MBC {
    public:
        explicit NoMBC(std::vector<uint8_t> rom);

        uint8_t read_rom(uint16_t address) const override;
        void write_rom(uint16_t address, uint8_t value) override;

        uint8_t read_ram(uint16_t address) const override;
        void write_ram(uint16_t address, uint8_t value) override;

    private:
        std::vector<uint8_t> rom_;
};
