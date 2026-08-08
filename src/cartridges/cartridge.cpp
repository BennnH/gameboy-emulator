#include "cartridge.h"
#include "no_mbc.h"
#include "mbc5.h"
#include <fstream>


// Cartridge type byte (0x0147) values
enum CartridgeType : uint8_t {
    CART_ROM_ONLY = 0x00,
    CART_MBC5 = 0x19,
    CART_MBC5_RAM = 0x1A,
    CART_MBC5_RAM_BATTERY = 0x1B,
    CART_MBC5_RUMBLE = 0x1C,
    CART_MBC5_RUMBLE_RAM = 0x1D,
    CART_MBC5_RUMBLE_RAM_BATTERY = 0x1E,
};


bool Cartridge::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open ROM: " + filename);
    }
    std::streamsize size = file.tellg();

    std::vector<uint8_t> rom(size);

    // Jump back to the start and read all bytes into the vector
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(rom.data()), size);

    cartridge_type_ = rom[0x0147];
    rom_size_code_  = rom[0x0148];
    ram_size_code_  = rom[0x0149];

    // Cartridge Title - stored at 0x0134-0x0143 as ASCII characters.
    title_.clear();
    for (uint16_t address = 0x0134; address <= 0x0143; address++) {
        char c = rom[address];
        if (c == 0) break;
        title_ += c;
    }
    // Pick the right class based on the cartridge type byte. For now we only
    // have no mbc, other types will be added when implemented.
    switch (cartridge_type_) {
        case CART_ROM_ONLY:
            mbc_ = std::make_unique<NoMBC>(std::move(rom));
            break;

        case CART_MBC5:
        case CART_MBC5_RAM:
        case CART_MBC5_RAM_BATTERY:
        case CART_MBC5_RUMBLE:
        case CART_MBC5_RUMBLE_RAM:
        case CART_MBC5_RUMBLE_RAM_BATTERY:
            mbc_ = std::make_unique<MBC5>(std::move(rom), ram_byte_size());
            break;

        default:
            // Falls back to default cartridge with no MBC
            mbc_ = std::make_unique<NoMBC>(std::move(rom));
            break;
    }
    return true;
}


uint8_t Cartridge::read(uint16_t address) const{
    return mbc_->read_rom(address);
}


void Cartridge::write(uint16_t address, uint8_t value) {
    mbc_->write_rom(address, value);
}


uint8_t Cartridge::read_ram(uint16_t address) const {
    return mbc_->read_ram(address);
}


void Cartridge::write_ram(uint16_t address, uint8_t value) {
    mbc_->write_ram(address, value);
}


int Cartridge::ram_byte_size() const {
    switch (ram_size_code_) {
        case 0x00: return 0;
        case 0x02: return 8 * 1024;    // 8 KiB
        case 0x03: return 32 * 1024;   // 32 KiB
        case 0x04: return 128 * 1024;  // 128 KiB
        case 0x05: return 64 * 1024;   // 64 KiB
        default:   return 0;
    }
}
