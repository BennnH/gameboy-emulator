#include "apu.h"

// The DMG runs at 4,194,304 T-cycles per second. The frame sequencer runs at
// 512Hz, so it steps once every 8192 cycles.
constexpr int CYCLES_PER_SEQUENCER_STEP = 8192;

constexpr int SAMPLE_RATE = 44100;
// This works out to about 95.1 cycles per sample, so it has to be a double.
// Truncating to 95 would generate samples slightly too fast and the audio
// would drift ahead of the emulator.
constexpr double CYCLES_PER_SAMPLE = 4194304.0 / SAMPLE_RATE;

// The four duty patterns a pulse channel can play. Each is 8 steps of on or
// off, and which one is selected changes the tone without changing the pitch.
constexpr uint8_t DUTY_TABLE[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},  // 12.5%
    {1, 0, 0, 0, 0, 0, 0, 1},  // 25%
    {1, 0, 0, 0, 0, 1, 1, 1},  // 50%
    {0, 1, 1, 1, 1, 1, 1, 0},  // 75%
};

// Base divisors for the noise channel's frequency timer, selected by the low
// 3 bits of NR43.
constexpr int NOISE_DIVISORS[8] = {8, 16, 32, 48, 64, 80, 96, 112};


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

    // Each channel's own timer runs at T-cycle speed and produces the waveform.
    tick_pulse(ch1_, cycles);
    tick_pulse(ch2_, cycles);
    tick_wave(cycles);
    tick_noise(cycles);

    // Downsample from the ~4MHz channel output to the 44.1kHz the sound card wants.
    sample_counter_ += cycles;
    while (sample_counter_ >= CYCLES_PER_SAMPLE) {
        sample_counter_ -= CYCLES_PER_SAMPLE;
        generate_sample();
    }
}


// Steps through the 8 stage cycle that drives everything which changes a
// channel's output over time, as opposed to producing the waveform itself.
void Apu::step_frame_sequencer() {
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
    if (address >= 0xFF30) {
        return wave_ram_[address - 0xFF30];
    }

    return registers_[address - 0xFF10];
}


void Apu::write_register(uint16_t address, uint8_t value) {
    // Wave RAM, which the game fills with the waveform shape it wants.
    if (address >= 0xFF30) {
        wave_ram_[address - 0xFF30] = value;
        return;
    }

    // NR52. Bit 7 is the master enable, but bits 0-3 are channel status driven
    // by the hardware and are read only, so a CPU write only touches bit 7.
    if (address == 0xFF26) {
        enabled_ = (value & 0x80) != 0;
        registers_[0xFF26 - 0xFF10] = (registers_[0xFF26 - 0xFF10] & 0x0F) | (value & 0x80);
        return;
    }

    registers_[address - 0xFF10] = value;

    switch (address) {
        // NR11, duty pattern and length.
        case 0xFF11:
            ch1_.duty_pattern = (value >> 6) & 0x03;
            ch1_.length_counter = 64 - (value & 0x3F);
            break;

        // NR12, envelope. The top 5 bits being clear switches the DAC off.
        case 0xFF12:
            ch1_.envelope_initial_volume = (value >> 4) & 0x0F;
            ch1_.envelope_increasing = (value & 0x08) != 0;
            ch1_.envelope_period = value & 0x07;
            ch1_.dac_enabled = (value & 0xF8) != 0;
            if (!ch1_.dac_enabled) {
                ch1_.enabled = false;
            }
            break;

        // NR13, low 8 bits of the frequency.
        case 0xFF13:
            ch1_.frequency = (ch1_.frequency & 0x0700) | value;
            break;

        // NR14, high 3 bits of the frequency plus trigger and length enable.
        case 0xFF14:
            ch1_.frequency = (ch1_.frequency & 0x00FF) | ((value & 0x07) << 8);
            ch1_.length_enabled = (value & 0x40) != 0;
            if (value & 0x80) {
                trigger_pulse(ch1_, true);
            }
            break;

        // NR21-NR24, channel 2. Identical to channel 1 without the sweep.
        case 0xFF16:
            ch2_.duty_pattern = (value >> 6) & 0x03;
            ch2_.length_counter = 64 - (value & 0x3F);
            break;

        case 0xFF17:
            ch2_.envelope_initial_volume = (value >> 4) & 0x0F;
            ch2_.envelope_increasing = (value & 0x08) != 0;
            ch2_.envelope_period = value & 0x07;
            ch2_.dac_enabled = (value & 0xF8) != 0;
            if (!ch2_.dac_enabled) {
                ch2_.enabled = false;
            }
            break;

        case 0xFF18:
            ch2_.frequency = (ch2_.frequency & 0x0700) | value;
            break;

        case 0xFF19:
            ch2_.frequency = (ch2_.frequency & 0x00FF) | ((value & 0x07) << 8);
            ch2_.length_enabled = (value & 0x40) != 0;
            if (value & 0x80) {
                trigger_pulse(ch2_, false);
            }
            break;

        // NR30, the wave channel's DAC is a single bit rather than a volume.
        case 0xFF1A:
            ch3_.dac_enabled = (value & 0x80) != 0;
            if (!ch3_.dac_enabled) {
                ch3_.enabled = false;
            }
            break;

        // NR31, length. The wave channel gets a full byte, so 256 steps.
        case 0xFF1B:
            ch3_.length_counter = 256 - value;
            break;

        // NR32, volume as a right shift. 0 mutes, then 100%, 50% and 25%.
        case 0xFF1C:
            switch ((value >> 5) & 0x03) {
                case 0: ch3_.volume_shift = 4; break;
                case 1: ch3_.volume_shift = 0; break;
                case 2: ch3_.volume_shift = 1; break;
                default: ch3_.volume_shift = 2; break;
            }
            break;

        case 0xFF1D:
            ch3_.frequency = (ch3_.frequency & 0x0700) | value;
            break;

        case 0xFF1E:
            ch3_.frequency = (ch3_.frequency & 0x00FF) | ((value & 0x07) << 8);
            ch3_.length_enabled = (value & 0x40) != 0;
            if (value & 0x80) {
                trigger_wave();
            }
            break;

        // NR41, length only, the noise channel has no duty pattern.
        case 0xFF20:
            ch4_.length_counter = 64 - (value & 0x3F);
            break;

        case 0xFF21:
            ch4_.envelope_initial_volume = (value >> 4) & 0x0F;
            ch4_.envelope_increasing = (value & 0x08) != 0;
            ch4_.envelope_period = value & 0x07;
            ch4_.dac_enabled = (value & 0xF8) != 0;
            if (!ch4_.dac_enabled) {
                ch4_.enabled = false;
            }
            break;

        // NR43. The noise channel has no frequency, just a clock divisor.
        case 0xFF22:
            ch4_.clock_shift = (value >> 4) & 0x0F;
            ch4_.width_mode = (value & 0x08) != 0;
            ch4_.divisor_code = value & 0x07;
            break;

        case 0xFF23:
            ch4_.length_enabled = (value & 0x40) != 0;
            if (value & 0x80) {
                trigger_noise();
            }
            break;

        default:
            break;
    }
}


void Apu::tick_pulse(PulseChannel& channel, int cycles) {
    channel.frequency_timer -= cycles;
    while (channel.frequency_timer <= 0) {
        // A higher frequency value means a smaller reload, so the duty position
        // advances faster and the pitch goes up.
        channel.frequency_timer += (2048 - channel.frequency) * 4;
        channel.duty_position = (channel.duty_position + 1) & 0x07;
    }
}


int Apu::pulse_output(const PulseChannel& channel) const {
    if (!channel.enabled || !channel.dac_enabled) {
        return 0;
    }
    return DUTY_TABLE[channel.duty_pattern][channel.duty_position] ? channel.volume : 0;
}

// Mixes the channel outputs into one stereo sample, applying the routing matrix
// in NR51 and the master volume in NR50.
void Apu::generate_sample() {
    int ch1 = pulse_output(ch1_);
    int ch2 = pulse_output(ch2_);
    int ch3 = wave_output();
    int ch4 = noise_output();

    uint8_t nr51 = registers_[0xFF25 - 0xFF10];
    uint8_t nr50 = registers_[0xFF24 - 0xFF10];

    // The low nibble of NR51 routes channels to the right, the high nibble
    // to the left.
    int left = 0;
    int right = 0;
    if (nr51 & 0x10) left += ch1;
    if (nr51 & 0x20) left += ch2;
    if (nr51 & 0x40) left += ch3;
    if (nr51 & 0x80) left += ch4;
    if (nr51 & 0x01) right += ch1;
    if (nr51 & 0x02) right += ch2;
    if (nr51 & 0x04) right += ch3;
    if (nr51 & 0x08) right += ch4;

    int left_volume = (nr50 >> 4) & 0x07;
    int right_volume = nr50 & 0x07;

    // Four channels of 0-15 gives a maximum of 60, and the master volume scales
    // that by a factor of 1/8 to 8/8.
    float left_sample = (left / 60.0f) * ((left_volume + 1) / 8.0f);
    float right_sample = (right / 60.0f) * ((right_volume + 1) / 8.0f);

    sample_buffer_.push_back(left_sample);
    sample_buffer_.push_back(right_sample);
}


void Apu::trigger_pulse(PulseChannel& channel, bool is_channel_1) {
    // Triggering a channel whose DAC is off leaves it silent.
    channel.enabled = channel.dac_enabled;

    if (channel.length_counter == 0) {
        channel.length_counter = 64;
    }
    channel.frequency_timer = (2048 - channel.frequency) * 4;
    channel.volume = channel.envelope_initial_volume;
    channel.envelope_counter = channel.envelope_period;

    // Sweep is channel 1 only, and gets set up in a later step. Delete once implemented!!
    (void)is_channel_1;
}


void Apu::tick_wave(int cycles) {
    ch3_.frequency_timer -= cycles;
    while (ch3_.frequency_timer <= 0) {
        // The wave channel steps twice as fast as a pulse channel, because it
        // has 32 samples to get through instead of 8 duty steps.
        ch3_.frequency_timer += (2048 - ch3_.frequency) * 2;
        ch3_.position = (ch3_.position + 1) & 0x1F;
    }
}


int Apu::wave_output() const {
    if (!ch3_.enabled || !ch3_.dac_enabled || ch3_.volume_shift > 3) {
        return 0;
    }

    // Two samples per byte, high nibble first.
    uint8_t packed = wave_ram_[ch3_.position / 2];
    int sample = (ch3_.position & 1) ? (packed & 0x0F) : (packed >> 4);
    return sample >> ch3_.volume_shift;
}


void Apu::trigger_wave() {
    ch3_.enabled = ch3_.dac_enabled;
    if (ch3_.length_counter == 0) {
        ch3_.length_counter = 256;
    }
    ch3_.frequency_timer = (2048 - ch3_.frequency) * 2;
    ch3_.position = 0;
}


void Apu::tick_noise(int cycles) {
    ch4_.frequency_timer -= cycles;
    while (ch4_.frequency_timer <= 0) {
        ch4_.frequency_timer += NOISE_DIVISORS[ch4_.divisor_code] << ch4_.clock_shift;

        // XOR the bottom two bits and feed the result back into the top.
        int feedback = (ch4_.lfsr & 1) ^ ((ch4_.lfsr >> 1) & 1);
        ch4_.lfsr >>= 1;
        ch4_.lfsr |= static_cast<uint16_t>(feedback << 14);

        // Width mode also feeds bit 6, which shortens the repeat period from
        // 32767 steps to 127 and makes the noise sound pitched.
        if (ch4_.width_mode) {
            ch4_.lfsr &= ~(1 << 6);
            ch4_.lfsr |= static_cast<uint16_t>(feedback << 6);
        }
    }
}


int Apu::noise_output() const {
    if (!ch4_.enabled || !ch4_.dac_enabled) {
        return 0;
    }
    // The output is bit 0 inverted.
    return (~ch4_.lfsr & 1) ? ch4_.volume : 0;
}


void Apu::trigger_noise() {
    ch4_.enabled = ch4_.dac_enabled;
    if (ch4_.length_counter == 0) {
        ch4_.length_counter = 64;
    }
    ch4_.frequency_timer = NOISE_DIVISORS[ch4_.divisor_code] << ch4_.clock_shift;
    ch4_.volume = ch4_.envelope_initial_volume;
    ch4_.envelope_counter = ch4_.envelope_period;
    // All bits set, so the sequence starts from the same place every time.
    ch4_.lfsr = 0x7FFF;
}
