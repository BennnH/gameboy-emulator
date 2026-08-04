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

    Display display(4);

    const int ms_per_frame = 1000 / 60;

    bool running = true;
    while (running) {
        uint32_t frame_start = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        gb.run_frame();
        display.render(gb.ppu().get_frame());

        uint32_t frame_duration = SDL_GetTicks() - frame_start;
        if (frame_duration < ms_per_frame) {
            SDL_Delay(ms_per_frame - frame_duration);
        }
    }
    return 0;
}
