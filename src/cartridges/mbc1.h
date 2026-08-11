#pragma once

#include "mbc.h"
#include <vector>
#include <cstdint>

class MBC1 : public MBC {
    public:
        explicit MBC1(std::vector<uint8_t> rom, int ram_size);

        uint8_t read_rom(uint16_t address) const override;
        void write_rom(uint16_t address, uint8_t value) override;

        uint8_t read_ram(uint16_t address) const override;
        void write_ram(uint16_t address, uint8_t value) override;

        const std::vector<uint8_t>& get_ram() const override;
        void load_ram(const std::vector<uint8_t>& data) override;

    private:
        std::vector<uint8_t> rom_;
        std::vector<uint8_t> ram_;

        uint8_t bank_lo_{1};        // BANK1: low 5 bits of ROM bank (0x2000-0x3FFF)
        uint8_t bank_hi_{0};        // BANK2: 2 bits, either ROM-bank-high or RAM-bank (0x4000-0x5FFF)
        bool mode_{false};          // banking mode select (0x6000-0x7FFF): false=ROM, true=RAM/advanced
        bool ram_enabled_{false};
};
