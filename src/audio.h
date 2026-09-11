#pragma once

#include <SDL.h>
#include <vector>


class Audio {
    public:
        Audio();
        ~Audio();

        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;

        void queue_samples(std::vector<float>& samples);

    private:
        SDL_AudioDeviceID device_{0};
};
