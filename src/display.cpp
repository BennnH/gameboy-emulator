#include "display.h"
#include <SDL.h>


Display::Display(int scale) {
    SDL_Init(SDL_INIT_VIDEO);

    window_ = SDL_CreateWindow(
        "GameBoy Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        160 * scale,
        144 * scale,
        SDL_WINDOW_SHOWN
    );

    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    texture_ = SDL_CreateTexture(
        renderer_,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        160,
        144
    );
}


Display::~Display() {
    SDL_DestroyTexture(texture_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}


void Display::render(const std::array<uint8_t, 160 * 144>& frame) {
    // DMG uses 0-3 shades making up the colour palette
    static const uint32_t palette[4] = {
        0xFFFFFFFF,
        0xAAAAAAFF,
        0x555555FF,
        0x000000FF
    };

    // Converts each pixel in the frame to its shade
    uint32_t pixels[160 * 144];
    for (int i = 0; i < 160 * 144; i++) {
        pixels[i] = palette[frame[i] & 0x03];
    }


    SDL_UpdateTexture(texture_, nullptr, pixels, 160 * sizeof(uint32_t));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}
