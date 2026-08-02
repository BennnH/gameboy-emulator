#pragma once


struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Display {
    public:
        explicit Display(int scale);
        ~Display();

    private:
        SDL_Window* window_{};
        SDL_Renderer* renderer_{};
        SDL_Texture* texture_{};
};
