#include "gb.h"
#include "display.h"
#include <iostream>
#include <SDL.h>


constexpr uint8_t BTN_RIGHT = 0x01;
constexpr uint8_t BTN_LEFT = 0x02;
constexpr uint8_t BTN_UP = 0x04;
constexpr uint8_t BTN_DOWN = 0x08;
constexpr uint8_t BTN_A = 0x10;
constexpr uint8_t BTN_B = 0x20;
constexpr uint8_t BTN_SELECT = 0x40;
constexpr uint8_t BTN_START = 0x80;

uint8_t key_to_button(SDL_Keycode key) {
    switch (key) {
        case SDLK_RIGHT: return BTN_RIGHT;
        case SDLK_LEFT: return BTN_LEFT;
        case SDLK_UP: return BTN_UP;
        case SDLK_DOWN: return BTN_DOWN;
        case SDLK_z: return BTN_A;
        case SDLK_x: return BTN_B;
        case SDLK_RSHIFT: return BTN_SELECT;
        case SDLK_RETURN: return BTN_START;
        default: return 0;
    }
}

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

    // The DMG runs at ~59.7275 FPS (4194304 cycles/sec / 70224 cycles/frame).
    const double ms_per_frame = 1000.0 / 59.7275;
    double next_frame_time = SDL_GetTicks();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                uint8_t btn = key_to_button(event.key.keysym.sym);
                if (btn) {
                    gb.bus().set_button_state(btn, event.type == SDL_KEYDOWN);
                }
            }
        }

        gb.run_frame();
        display.render(gb.ppu().get_frame());

        next_frame_time += ms_per_frame;
        double current_ticks = SDL_GetTicks();
        if (current_ticks < next_frame_time) {
            SDL_Delay(static_cast<uint32_t>(next_frame_time - current_ticks));
        } else {
            // We're behind (e.g. the window was dragged, or a frame ran long),
            // so resync rather than trying to catch up.
            next_frame_time = current_ticks;
        }
    }
    gb.write_save();
    return 0;
}
