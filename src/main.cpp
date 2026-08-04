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

    // Testing Display
    Display display(4);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        gb.run_frame();
        display.render(gb.ppu().get_frame());
    }

    return 0;
}
