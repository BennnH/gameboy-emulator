#pragma once

#include <cstdint>
#include <array>
#include <vector>



// A square wave channel. Channels 1 and 2 are identical except that only
// channel 1 has a frequency sweep unit, so channel 2 leaves those fields unused.
struct PulseChannel {
    // Set by a trigger, cleared when the length counter runs out or the DAC
    // is switched off.
    bool enabled{false};
    // The DAC is off when the top 5 bits of NR12 are clear, which silences the
    // channel regardless of everything else.
    bool dac_enabled{false};

    // Which of the four duty patterns we're playing, and where we are in it.
    int duty_pattern{0};
    int duty_position{0};

    // 11 bit frequency, split across NR13 (low 8 bits) and NR14 (high 3 bits).
    int frequency{0};
    // Counts down every T-cycle. On reaching zero it reloads and the duty
    // position advances by one, which is what actually produces the pitch.
    int frequency_timer{0};

    // Volume envelope, clocked at 64Hz by the frame sequencer.
    int volume{0};
    int envelope_initial_volume{0};
    bool envelope_increasing{false};
    int envelope_period{0};
    int envelope_counter{0};

    // Length counter, clocked at 256Hz. Only switches the channel off when the
    // length enable bit in NR14 is set, which games often leave clear.
    int length_counter{0};
    bool length_enabled{false};

    // Frequency sweep, clocked at 128Hz. Channel 1 only.
    int sweep_period{0};
    bool sweep_decreasing{false};
    int sweep_shift{0};
    int sweep_counter{0};
    int sweep_shadow{0};
    bool sweep_enabled{false};
};


// Plays 32 four bit samples held in wave RAM, so unlike the pulse channels the
// waveform shape is whatever the game puts there.
struct WaveChannel {
    bool enabled{false};
    bool dac_enabled{false};

    int frequency{0};
    int frequency_timer{0};
    // Which of the 32 samples we're currently on.
    int position{0};

    // Volume is a right shift rather than a multiplier. 0 is full volume, 1 is
    // half, 2 is a quarter, and 4 is our marker for muted.
    int volume_shift{4};

    int length_counter{0};
    bool length_enabled{false};
};


// Pseudo random noise from a shift register, used for percussion and effects.
struct NoiseChannel {
    bool enabled{false};
    bool dac_enabled{false};

    // 15 bit linear feedback shift register. Feeding the result back into bit 6
    // as well shortens the period and makes it sound much more tonal.
    uint16_t lfsr{0x7FFF};
    bool width_mode{false};

    int frequency_timer{0};
    int clock_shift{0};
    int divisor_code{0};

    int volume{0};
    int envelope_initial_volume{0};
    bool envelope_increasing{false};
    int envelope_period{0};
    int envelope_counter{0};

    int length_counter{0};
    bool length_enabled{false};
};


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
        PulseChannel ch1_{};
        PulseChannel ch2_{};

        void tick_pulse(PulseChannel& channel, int cycles);
        int pulse_output(const PulseChannel& channel) const;
        void trigger_pulse(PulseChannel& channel, bool is_channel_1);
        void generate_sample();

        WaveChannel ch3_{};

        // Wave RAM at 0xFF30-0xFF3F, holding 32 samples packed two per byte.
        std::array<uint8_t, 16> wave_ram_{};

        void tick_wave(int cycles);
        int wave_output() const;
        void trigger_wave();


        NoiseChannel ch4_{};

        void tick_noise(int cycles);
        int noise_output() const;
        void trigger_noise();
};
