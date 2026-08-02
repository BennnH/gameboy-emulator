#pragma once

#include <array>
#include <cstdint>


struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Display {
    public:
        explicit Display(int scale);
        ~Display();

        void render(const std::array<uint8_t, 160 * 144>& frame);

    private:
        SDL_Window* window_{};
        SDL_Renderer* renderer_{};
        SDL_Texture* texture_{};
};
