#include "cartridges/cartridge.h"
#include "bus.h"
#include "cpu.h"

#include <iostream>

int main(int argc, char* argv[]) {
    Cartridge cart;
    if (!cart.load("ROMs/pokemon_yellow.gb")) {
        std::cout << "Failed to load ROM\n";
        return 1;
    }

    std::cout << "ROM loaded successfully!\n";
    std::cout << "Title: " << cart.title() << "\n";
    std::cout << "Cartridge type: " << static_cast<int>(cart.cartridge_type()) << "\n";
    std::cout << "ROM size code: "  << static_cast<int>(cart.rom_size_code()) << "\n";
    std::cout << "RAM size code: "  << static_cast<int>(cart.ram_size_code()) << "\n";

    Bus bus(cart);
    CPU cpu(bus);

    return 0;
}
