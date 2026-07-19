#include "gb.h"
#include <iostream>

int main() {
    Gameboy gb;

    if (!gb.load_rom("ROMs/06-ld r,r.gb")) {
        std::cout << "Failed to load ROM\n";
        return 1;
    }

    std::cout << "Running test ROM...\n";
    gb.run(10000000);
    std::cout << "\nDone.\n";

    return 0;
}
