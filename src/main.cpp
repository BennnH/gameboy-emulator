#include "gb.h"
#include "display.h"
#include <iostream>
#include <SDL.h>

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

    // Testing Display
    Display display(4);

    // Testing one frame cycling across the shades.
    std::array<uint8_t, 160 * 144> test_frame{};
    for (int i = 0; i < 160 * 144; i++) {
        test_frame[i] = (i / 40) % 4;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        display.render(test_frame);
    }

    return 0;
}
