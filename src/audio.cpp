#include "audio.h"


Audio::Audio() {
    // The Display constructor already brought SDL up with SDL_Init, so we only
    // need to add the audio subsystem on top of it.
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    SDL_AudioSpec want{};
    want.freq = 44100;
    // 32 bit floats in native byte order, which is what the APU produces.
    want.format = AUDIO_F32SYS;
    want.channels = 2;
    // Buffer size in sample frames, roughly 12ms of audio at 44.1kHz.
    want.samples = 512;
    // A null callback selects the queue based API instead of SDL calling us
    // back on its own thread, which keeps everything on the emulator thread.
    want.callback = nullptr;

    SDL_AudioSpec have{};
    device_ = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);

    // Devices open in a paused state, so nothing plays until we unpause.
    if (device_ != 0) {
        SDL_PauseAudioDevice(device_, 0);
    }
}


Audio::~Audio() {
    if (device_ != 0) {
        SDL_CloseAudioDevice(device_);
    }
}


void Audio::queue_samples(std::vector<float>& samples) {
    // A device id of 0 means the device failed to open, in which case we just
    // run silently rather than stopping the whole emulator.
    if (device_ == 0 || samples.empty()) {
        return;
    }

    // SDL wants a size in bytes rather than a count of samples.
    SDL_QueueAudio(device_, samples.data(), static_cast<uint32_t>(samples.size() * sizeof(float)));
    samples.clear();
}
