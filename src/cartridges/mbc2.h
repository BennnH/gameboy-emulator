#pragma once

#include "mbc.h"
#include <vector>
#include <cstdint>


class MBC2 : public MBC {
    public:
        explicit MBC2(std::vector<uint8_t> rom, int ram_size);

        uint8_t read_rom(uint16_t address) const override;
        void write_rom(uint16_t address, uint8_t value) override;

        uint8_t read_ram(uint16_t address) const override;
        void write_ram(uint16_t address, uint8_t value) override;

        const std::vector<uint8_t>& get_ram() const override;
        void load_ram(const std::vector<uint8_t>& data) override;

    private:
        std::vector<uint8_t> rom_;
        std::vector<uint8_t> ram_;

        uint8_t rom_bank_{1};
        bool ram_enabled_{false};


};
