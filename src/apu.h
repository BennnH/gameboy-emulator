#pragma once

#include <cstdint>
#include <array>
#include <vector>


class Apu {
    public:
        void reset();
        void tick(int cycles);

        uint8_t read_register(uint16_t address) const;
        void write_register(uint16_t address, uint8_t value);

        std::vector<float>& sample_buffer() { return sample_buffer_; }

    private:
        std::array<uint8_t, 0x30> registers_{};

        int sequencer_counter_{0};
        int sequencer_step_{0};
        void step_frame_sequencer();

        double sample_counter_{0.0};
        std::vector<float> sample_buffer_;

        bool enabled_{false};
};
