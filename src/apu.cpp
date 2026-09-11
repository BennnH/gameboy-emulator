#include "apu.h"
#include <cstdio>

// The DMG runs at 4,194,304 T-cycles per second. The frame sequencer runs at
// 512Hz, so it steps once every 8192 cycles.
constexpr int CYCLES_PER_SEQUENCER_STEP = 8192;

constexpr int SAMPLE_RATE = 44100;
// This works out to about 95.1 cycles per sample, so it has to be a double.
// Truncating to 95 would generate samples slightly too fast and the audio
// would drift ahead of the emulator.
constexpr double CYCLES_PER_SAMPLE = 4194304.0 / SAMPLE_RATE;


void Apu::reset() {
    registers_.fill(0);
    sequencer_counter_ = 0;
    sequencer_step_ = 0;
    sample_counter_ = 0.0;
    sample_buffer_.clear();
    enabled_ = false;
}


void Apu::tick(int cycles) {
    // The frame sequencer clocks the length counters, sweep and envelope. It
    // keeps running regardless of which channels are currently active.
    sequencer_counter_ += cycles;
    while (sequencer_counter_ >= CYCLES_PER_SEQUENCER_STEP) {
        sequencer_counter_ -= CYCLES_PER_SEQUENCER_STEP;
        step_frame_sequencer();
    }

    // TODO: tick each channel's frequency timer, then take an output sample
    // every CYCLES_PER_SAMPLE cycles. Nothing to sample until channel 1 exists.
}


// Steps through the 8 stage cycle that drives everything which changes a
// channel's output over time, as opposed to producing the waveform itself.
void Apu::step_frame_sequencer() {
    static int steps = 0;
    if (++steps % 512 == 0) {
        std::printf("sequencer: %d seconds\n", steps / 512);
        std::fflush(stdout);
    }
    switch (sequencer_step_) {
        // Length counters are clocked on every even step, giving 256Hz.
        case 0:
        case 4:
            // TODO: clock length counters.
            break;

        // Steps 2 and 6 clock length as above, and also the sweep at 128Hz.
        case 2:
        case 6:
            // TODO: clock length counters and channel 1's sweep.
            break;

        // The envelope is clocked once per full cycle, giving 64Hz.
        case 7:
            // TODO: clock envelopes.
            break;

        default:
            break;
    }
    sequencer_step_ = (sequencer_step_ + 1) & 0x07;
}


uint8_t Apu::read_register(uint16_t address) const {
    return registers_[address - 0xFF10];
}


void Apu::write_register(uint16_t address, uint8_t value) {
    // NR52. Bit 7 is the master enable, but bits 0-3 are channel status driven
    // by the hardware and are read only, so a CPU write only touches bit 7.
    if (address == 0xFF26) {
        enabled_ = (value & 0x80) != 0;
        registers_[0xFF26 - 0xFF10] = (registers_[0xFF26 - 0xFF10] & 0x0F) | (value & 0x80);
        return;
    }

    registers_[address - 0xFF10] = value;
}
