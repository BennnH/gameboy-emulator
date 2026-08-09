#pragma once

#include "mbc.h"

#include <cstdint>
#include <string>
#include <memory>

class Cartridge {
    public:
        bool load(const std::string& filename);
        uint8_t read(uint16_t address) const;
        void write(uint16_t address, uint8_t value);

        uint8_t read_ram(uint16_t address) const;
        void write_ram(uint16_t address, uint8_t value);

        const std::string& title() const { return title_; }
        uint8_t cartridge_type() const { return cartridge_type_; }
        uint8_t rom_size_code() const { return rom_size_code_; }
        uint8_t ram_size_code() const { return ram_size_code_; }

        void write_save() const;
        void read_save();

    private:
        std::unique_ptr<MBC> mbc_;

        int ram_byte_size() const;

        bool has_battery() const;

        // Header info
        std::string title_;
        uint8_t cartridge_type_{};
        uint8_t rom_size_code_{};
        uint8_t ram_size_code_{};

        std::string save_path_;
};
