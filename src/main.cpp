#include "gb.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: gameboy <rom>\n";
        return 1;
    }

    Gameboy gb;

    if (!gb.load_rom(argv[1])) {
        std::cout << "Failed to load ROM\n";
        return 1;
    }

    std::cout << "Running test ROM...\n";
    gb.run(10000000);
    std::cout << "\nDone.\n";

    return 0;
}
