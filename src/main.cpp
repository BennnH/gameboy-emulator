#include "cartridges/cartridge.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Cartridge cart;
    if (!cart.load("ROMs/tetris.gb")) {
        std::cout << "Failed to load ROM\n";
        return 1;
    }

    std::cout << "ROM loaded successfully!\n";

    // Cartridge title is stored at 0x0134-0x0143 as ASCII characters
    // Using for test right now.
    std::cout << "Title: ";
    for (uint16_t address = 0x0134; address <=0x0143; address++) {
        char c = cart.read(address);
        if (c == 0) {
            break;
        }
        std::cout << c;
    }
    std::cout << "\n";

    // The cartridge type byte is at 0x0147
    std::cout << "Cartridge type: " <<  static_cast<int>(cart.read(0x0147)) << "\n";

    return 0;
}
