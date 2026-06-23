#include "cartridge.h"
#include <fstream>


bool Cartridge::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open ROM: " + filename);
    }
    std::streamsize size = file.tellg();

    // Resize the vector to hold all of ROM
    rom_.resize(size);

    // Jump back to the start and read all bytes into the vector
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(rom_.data()), size);

    parseheader();
    return true;
}

uint8_t Cartridge::read(uint16_t address) const{
    return rom_[address];
}

void Cartridge::parseheader() {
    // Cartridge Title - stored at 0x0134-0x0143 as ASCII characters.
    title_.clear();
    for (uint16_t address = 0x0134; address <=0x0143; address++) {
        char c = rom_[address];
        if (c == 0) {
            break;
        }
        title_ += c;
    }

    cartridge_type_ = rom_[0x0147];
    rom_size_code_ = rom_[0x0148];
    ram_size_code_ = rom_[0x0149];
}
