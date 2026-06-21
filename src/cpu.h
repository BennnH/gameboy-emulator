#pragma once

#include <cstdint>

class Bus;

class CPU {
    public:
        explicit CPU(Bus& bus);
        void step();

    private:
        // Flag bit masks (more explainable than magic nums in code)
        static constexpr uint8_t FLAG_Z = 0x80;
        static constexpr uint8_t FLAG_N = 0x40;
        static constexpr uint8_t FLAG_H = 0x20;
        static constexpr uint8_t FLAG_C = 0x10;

        // Registers
        uint8_t a_{};
        uint8_t b_{};
        uint8_t c_{};
        uint8_t d_{};
        uint8_t e_{};
        uint8_t h_{};
        uint8_t l_{};
        uint8_t f_{};

        uint16_t sp_{};
        uint16_t pc_{};

        // Can access registers as higher and lower bytes of pairs, so for now just use getters and setters to
        // update individual registers

        uint16_t get_af() const {
            return (a_ << 8) | f_;
        }

        uint16_t get_bc() const {
            return (b_ << 8) | c_;
        }

        uint16_t get_de() const {
            return (d_ << 8) | e_;
        }

        uint16_t get_hl() const {
            return (h_ << 8) | l_;
        }

        // setters
        void set_af(uint16_t value) {
            a_ = value >> 8;
            // Lower nibble of F is always zero
            f_ = value & 0xF0;
        }

        void set_bc(uint16_t value) {
            b_ = value >> 8;
            c_ = value & 0xFF;
        }
        void set_de(uint16_t value) {
            d_ = value >> 8;
            e_ = value & 0xFF;
        }
        void set_hl(uint16_t value) {
            h_ = value >> 8;
            l_ = value & 0xFF;
        }

        void add(uint8_t value);
        void adc(uint8_t value);
        void sub(uint8_t value);
        void sbc(uint8_t value);

        Bus& bus_;

};
