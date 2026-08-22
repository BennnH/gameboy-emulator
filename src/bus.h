#pragma once

#include <array>
#include <cstdint>

class Cartridge;

class Bus {
    public:
        explicit Bus(Cartridge& cartridge);
        void reset();

        uint8_t read8(uint16_t address) const;
        void write8(uint16_t address, uint8_t value);

        void set_button_state(uint8_t button_mask, bool pressed);

        uint8_t read_io(uint16_t address) const;
        void write_io(uint16_t address, uint8_t value);

        void tick(int cycles);

        int get_tac_speed() const;
        void perform_oam_dma(uint8_t value);
    private:
        Cartridge& cartridge_;
        // Memory regions from pandocs memory map.

        // 0x8000-0x9FFF
        std::array<uint8_t, 0x2000> vram_{};
        // 0xC000-0xDFFF
        std::array<uint8_t, 0x2000> wram_{};
        // 0xFE00-0xFE9F
        std::array<uint8_t, 0xA0> oam_{};
        // 0xFF00-0xFF7F
        std::array<uint8_t, 0x80> io_{};
        // 0xFF80-0xFFFE
        std::array<uint8_t, 0x7F> hram_{};
        // 0xFFFF - Interrupt Enable register
        uint8_t ie_{};

        // Total t cycles since boot
        uint64_t cycles_{};

        int div_counter_{};
        int tima_counter_{};

        uint8_t button_state_{0x00};
        // Last computed low nibble of P1, for edge-detecting the joypad interrupt.
        uint8_t prev_joypad_bits_{0x0F};

        uint8_t joypad_bits() const;
        void update_joypad_interrupt();

};
