#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Cartridge {
    public:
        bool load(const std::string& filename);
        uint8_t read(uint16_t address) const;

        // Header info accessors for debugging
        const std::string& title() const { return title_; }
        uint8_t cartridge_type() const { return cartridge_type_; }
        uint8_t rom_size_code() const { return rom_size_code_; }
        uint8_t ram_size_code() const { return ram_size_code_; }

    private:
        void parseheader();

        std::vector<uint8_t> rom_;

        // Header info
        std::string title_;
        uint8_t cartridge_type_{};
        uint8_t rom_size_code_{};
        uint8_t ram_size_code_{};
};
