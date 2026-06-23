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

    return true;
}

uint8_t Cartridge::read(uint16_t address) const{
    return rom_[address];
}
