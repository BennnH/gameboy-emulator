#pragma once

#include <cstdint>
#include <vector>


class MBC {
    public:
        virtual ~MBC() = default;

        // Writes to the ROM region (0x0000-0x7FFF)
        virtual uint8_t read_rom(uint16_t address) const = 0;
        virtual void write_rom(uint16_t address, uint8_t value) = 0;

        // Cartridge RAM region (0xA000-0xBFFF)
        virtual uint8_t read_ram(uint16_t address) const = 0;
        virtual void write_ram(uint16_t address, uint8_t value) = 0;

        // Save support. Default implementations mean mappers without battery
        virtual bool has_battery() const { return false; }
        virtual const std::vector<uint8_t>& get_ram() const { return empty_ram_; }
        virtual void load_ram(const std::vector<uint8_t>&) {}

    private:
        inline static const std::vector<uint8_t> empty_ram_{};


};
