#include "cartridge.h"
#include "no_mbc.h"
#include <fstream>


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
        case 0x00:
            mbc_ = std::make_unique<NoMBC>(std::move(rom));
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
