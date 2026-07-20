#pragma once

#include <cstdint>

class Bus;

class CPU {
    public:
        explicit CPU(Bus& bus);
        void reset();
        void print_state() const;
        // Debug only - lets tests point the CPU at hand-written code
        void set_pc(uint16_t value) { pc_ = value; }
        int step();

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

        bool ime_{};   // Interrupt Master Enable

        // Current num of t-cycles used by an instruction and returned at the end of a step.
        int cycles_{};

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
        void and_(uint8_t value);
        void xor_(uint8_t value);
        void or_(uint8_t value);
        void cp(uint8_t value);
        void inc(uint8_t& reg);
        void dec(uint8_t& reg);
        void jr_conditional(bool condition);
        void add_hl(uint16_t value);
        void push16(uint16_t value);
        uint16_t pop16();
        void jp_conditional(bool condition);
        void call_conditional(bool condition);
        void ret_conditional(bool condition);
        void rst(uint16_t vector);
        uint16_t add_sp_i8();
        uint8_t rlc(uint8_t value);
        uint8_t rrc(uint8_t value);
        uint8_t rl(uint8_t value);
        uint8_t rr(uint8_t value);
        uint8_t sla(uint8_t value);
        uint8_t sra(uint8_t value);
        uint8_t swap(uint8_t value);
        uint8_t srl(uint8_t value);
        void bit(uint8_t bit_number, uint8_t value);
        void execute_cb(uint8_t opcode);
        uint16_t read_u16();

        Bus& bus_;

};
