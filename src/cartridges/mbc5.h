#pragma once

#include "mbc.h"
#include <vector>
#include <cstdint>

class MBC5 : public MBC {
    public:
        explicit MBC5(std::vector<uint8_t> rom, int ram_size);

        uint8_t read_rom(uint16_t address) const override;
        void write_rom(uint16_t address, uint8_t value) override;

        uint8_t read_ram(uint16_t address) const override;
        void write_ram(uint16_t address, uint8_t value) override;

    private:
        std::vector<uint8_t> rom_;
        std::vector<uint8_t> ram_;

        uint16_t rom_bank_{1};
        uint8_t ram_bank_{0};
        bool ram_enabled_{false};
};
