#include "cpu.h"
#include "bus.h"
#include <cstdio>

CPU::CPU(Bus& bus) : bus_(bus) {

}

void CPU::reset() {
    // Post-boot-ROM state. (Nintendo logo scroll skipped) set registers to default values.
    set_af(0x01B0);
    set_bc(0x0013);
    set_de(0x00D8);
    set_hl(0x014D);
    sp_ = 0xFFFE;
    pc_ = 0x0100;
    ime_ = false;
}

void CPU::print_state() const {
    std::printf("A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X\n",
        a_, f_, b_, c_, d_, e_, h_, l_, sp_, pc_);
}

void CPU::add(uint8_t value) {
    // We only add and store values to register A (accumulator) and need to update flag register
    uint16_t result = a_ + value;
    uint8_t truncated = result & 0xFF;
    f_ = 0;

    if (truncated == 0) {
        f_ |= FLAG_Z;
    }
    if ((a_ & 0x0F) + (value & 0x0F) > 0x0F) {
        f_ |= FLAG_H;
    }
    if (result > 0xFF) {
        f_ |= FLAG_C;
    }

    a_ = truncated;
}

void CPU::adc(uint8_t value) {
    uint8_t carry = (f_ & FLAG_C) >> 4;
    uint16_t result = a_ + value + carry;
    uint8_t truncated = result & 0xFF;

    f_ = 0;
    if (truncated == 0) {
        f_ |= FLAG_Z;
    }
    if ((a_ & 0x0F) + (value & 0x0F) + carry > 0x0F) {
        f_ |= FLAG_H;
    }
    if (result > 0xFF) {
        f_ |= FLAG_C;
    }

    a_ = truncated;
}

void CPU::sub(uint8_t value) {
    uint8_t result = a_ - value;

    f_ = 0;
    f_ |= FLAG_N;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (value > a_) {
        f_ |= FLAG_C;
    }
    if ((value & 0x0F) > (a_ & 0x0F)) {
        f_ |= FLAG_H;
    }

    a_ = result;
}

void CPU::sbc(uint8_t value) {
    uint8_t carry = (f_ & FLAG_C) >> 4;
    int result = a_ - value - carry;

    f_ = 0;
    f_ |= FLAG_N;
    if ((result & 0xFF) == 0) {
        f_ |= FLAG_Z;
    }
    if ((value & 0x0F) + carry > (a_ & 0x0F)) {
        f_ |= FLAG_H;
    }
    if (result < 0) {
        f_ |= FLAG_C;
    }

    a_= result & 0xFF;
}

void CPU::and_(uint8_t value) {
    a_ &= value;
    f_ = 0;
    if (a_ == 0) {
        f_ |= FLAG_Z;
    }
    f_ |= FLAG_H;
}

void CPU::xor_(uint8_t value) {
    a_ ^= value;
    f_ = 0;
    if (a_ == 0) {
        f_ |= FLAG_Z;
    }
}

void CPU::or_(uint8_t value) {
    a_ |= value;
    f_= 0;
    if (a_ == 0) {
        f_ |= FLAG_Z;
    }
}

void CPU::cp(uint8_t value) {
    uint8_t result = a_ - value;

    f_ = 0;
    f_ |= FLAG_N;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (value > a_) {
        f_ |= FLAG_C;
    }
    if ((value & 0x0F) > (a_ & 0x0F)) {
        f_ |= FLAG_H;
    }
}

void CPU::inc(uint8_t& reg) {
    uint8_t result = reg + 1;

    // Want to reset everything but the carry flag.
    f_ &= FLAG_C;

    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if ((reg & 0x0F) == 0x0F) {
        f_ |= FLAG_H;
    }
    reg = result;
}

void CPU::dec(uint8_t& reg) {
    uint8_t result = reg - 1;

    // Want to reset everything but the carry flag.
    f_ &= FLAG_C;
    f_ |= FLAG_N;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if ((reg & 0x0F) == 0x00) {
        f_ |= FLAG_H;
    }
    reg = result;
}

void CPU::jr_conditional(bool condition) {
    // Have to cast to signed int so we can have -128 -> +127 range of relative jumps.
    // Relative so we add the value to the pc instead of just setting pc to an address.
    int8_t offset = static_cast<int8_t>(bus_.read8(pc_));
    pc_++;
    if (condition) {
        pc_ += offset;
    }
}

void CPU::add_hl(uint16_t value) {
    uint32_t result = get_hl() + value;

    // Preserve Zero flag
    f_ &= FLAG_Z;
    if ((get_hl() & 0x0FFF) + (value &  0x0FFF) > 0x0FFF) {
        f_ |= FLAG_H;
    }
    if (result > 0xFFFF) {
        f_ |= FLAG_C;
    }
    set_hl(result & 0xFFFF);
}

void CPU::push16(uint16_t value) {
    // Stack grows downward. High byte goes at the higher address,
    // so it gets pushed first, leaving the low byte at the lower address.
    sp_--;
    bus_.write8(sp_, value >> 8);
    sp_--;
    bus_.write8(sp_, value & 0xFF);
}

uint16_t CPU::pop16() {
    uint8_t low = bus_.read8(sp_);
    sp_++;
    uint8_t high = bus_.read8(sp_);
    sp_++;
    return (high << 8) | low;
}

void CPU::jp_conditional(bool condition) {
    uint16_t address = read_u16();
    if (condition) {
        pc_ = address;
    }
}

void CPU::call_conditional(bool condition) {
    uint16_t address = read_u16();
    if (condition) {
        push16(pc_);
        pc_ = address;
    }
}

void CPU::ret_conditional(bool condition) {
    if (condition) {
        pc_ = pop16();
    }
}

void CPU::rst(uint16_t vector) {
    push16(pc_);
    pc_ = vector;
}

uint16_t CPU::add_sp_i8() {
    uint8_t raw = bus_.read8(pc_);
    pc_++;
    int8_t offset = static_cast<int8_t>(raw);

    // Z and N are always cleared here, even though this is an addition.
    // H and C come from UNSIGNED low-byte arithmetic, not the signed result.
    f_ = 0;
    if ((sp_ & 0x0F) + (raw & 0x0F) > 0x0F) {
        f_ |= FLAG_H;
    }
    if ((sp_ & 0xFF) + raw > 0xFF) {
        f_ |= FLAG_C;
    }

    return sp_ + offset;
}

uint16_t CPU::read_u16() {
    uint8_t low = bus_.read8(pc_);
    pc_++;
    uint8_t high = bus_.read8(pc_);
    pc_++;
    return (high << 8) | low;
}

void CPU::step() {
    uint8_t opcode = bus_.read8(pc_);
    pc_++;

    switch (opcode) {
        // NOP
        case 0x00:
            break;
        //LD BC,u16
        case 0x01:
            set_bc(read_u16());
            break;
        // LD (BC),A
        case 0x02:
            bus_.write8(get_bc(), a_);
            break;
        // INC BC
        case 0x03:
            set_bc(get_bc() + 1);
            break;
        // INC B
        case 0x04:
            inc(b_);
            break;
        // DEC B
        case 0x05:
            dec(b_);
            break;
        // LD B,u8
        case 0x06:
            b_ = bus_.read8(pc_);
            pc_++;
            break;
        // RLCA - Rotate Left Circular A
        case 0x07: {
            uint8_t top_bit = (a_  >> 7) & 1;
            a_ = (a_ << 1) | top_bit;
            // Set all regs to 0, if top bit fell off then turn carry on.
            f_ = 0;
            if (top_bit) f_ |= FLAG_C;
            break;
        }
        // LD (u16),SP
        case 0x08: {
            uint16_t address = read_u16();
            bus_.write8(address, sp_ & 0xFF);
            bus_.write8(address + 1, sp_ >>  8);
            break;
        }

        // ADD HL,BC
        case 0x09:
            add_hl(get_bc());
            break;
        // LD A,(BC)
        case 0x0A:
            a_ = bus_.read8(get_bc());
            break;
        // DEC BC
        case 0x0B:
            set_bc(get_bc() - 1);
            break;
        // INC C
        case 0x0C:
            inc(c_);
            break;
        // DEC C
        case 0x0D:
            dec(c_);
            break;
        // LD C,u8
        case 0x0E:
            c_ = bus_.read8(pc_);
            pc_++;
            break;
        // RRCA - Rotate Right Circular A
        case 0x0F: {
            uint8_t bottom_bit = a_ & 1;
            a_ = (a_ >> 1) |(bottom_bit << 7);
            f_ = 0;
            if (bottom_bit) f_ |= FLAG_C;
            break;
        }
        // STOP - COME BACK TO THIS
        case 0x10:
            pc_++;
            break;
        // LD (DE),u16
        case 0x11:
            set_de(read_u16());
            break;
        // LD (DE),A
        case 0x12:
            bus_.write8(get_de(), a_);
            break;
        // INC DE
        case 0x13:
            set_de(get_de() + 1);
            break;
        // INC D
        case 0x14:
            inc(d_);
            break;
        // DEC D
        case 0x15:
            dec(d_);
            break;
        // LD D,u8
        case 0x16:
            d_ = bus_.read8(pc_);
            pc_++;
            break;
        // RLA - Rotate Left through carry A
        case 0x17: {
            uint8_t old_carry = (f_ & FLAG_C) ? 1 : 0;
            uint8_t top_bit = (a_ >> 7) & 1;
            a_ = (a_ << 1) | old_carry;
            f_ = 0;
            if (top_bit) f_ |= FLAG_C;
            break;
        }
        // JR i8
        case 0x18:
            jr_conditional(true);
            break;
        // ADD HL,DE
        case 0x19:
            add_hl(get_de());
            break;
        // LD A,(DE)
        case 0x1A:
            a_ = bus_.read8(get_de());
            break;
        // DEC DE
        case 0x1B:
            set_de(get_de() - 1);
            break;
        // INC E
        case 0x1C:
            inc(e_);
            break;
        // DEC E
        case 0x1D:
            dec(e_);
            break;
        // LD E,u8
        case 0x1E:
            e_ = bus_.read8(pc_);
            pc_++;
            break;
        // RRA - Rotate Right through carry A
        case 0x1F: {
            uint8_t old_carry = (f_ & FLAG_C) ? 1 : 0;
            uint8_t bottom_bit = a_ & 1;
            a_ = (a_ >> 1) | (old_carry << 7);
            f_ = 0;
            if (bottom_bit) f_ |= FLAG_C;
            break;
        }
        // JR NZ,i8
        case 0x20:
            jr_conditional(!(f_ & FLAG_Z));
            break;
        // LD HL,u16
        case 0x21:
            set_hl(read_u16());
            break;
        // LD (HL+),A
        case 0x22:
            bus_.write8(get_hl(), a_);
            set_hl(get_hl() + 1);
            break;
        // INC HL
        case 0x23:
            set_hl(get_hl() + 1);
            break;
        // INC H
        case 0x24:
            inc(h_);
            break;
        // DEC H
        case 0x25:
            dec(h_);
            break;
        // LD H,u8
        case 0x26:
            h_ = bus_.read8(pc_);
            pc_++;
            break;
        // DAA - Decimal Adjust Accumulator (NO idea).
        case 0x27: {
            uint8_t adjust = 0;
            bool set_carry = false;

            // If last op was NOT a subtraction, adjust upward where digits overflowed
            if (!(f_ & FLAG_N)) {
                if ((f_ & FLAG_H) || (a_ & 0x0F) > 0x09) {
                    adjust |= 0x06;
                }
                if ((f_ & FLAG_C) || a_ > 0x99) {
                    adjust |= 0x60;
                    set_carry = true;
                }
                a_ += adjust;
            } else {
                // Last op was a subtraction: adjust downward
                if (f_ & FLAG_H) {
                    adjust |= 0x06;
                }
                if (f_ & FLAG_C) {
                    adjust |= 0x60;
                    set_carry = true;
                }
                a_ -= adjust;
            }

            // Flags: N preserved, H always cleared, Z if result 0, C if high correction applied
            f_ &= FLAG_N;
            if (a_ == 0)   f_ |= FLAG_Z;
            if (set_carry) f_ |= FLAG_C;
            break;
        }
        //JR Z,i8
        case 0x28:
            jr_conditional(f_ & FLAG_Z);
            break;

        // ADD HL,HL
        case 0x29:
            add_hl(get_hl());
            break;
        // LD A,(HL+)
        case 0x2A:
            a_ = bus_.read8(get_hl());
            set_hl(get_hl() + 1);
            break;
        // DEC HL
        case 0x2B:
            set_hl(get_hl() - 1);
            break;
        // INC L
        case 0x2C:
            inc(l_);
            break;
        // DEC L
        case 0x2D:
            dec(l_);
            break;
        // LD L,u8
        case 0x2E:
            l_ = bus_.read8(pc_);
            pc_++;
            break;
        // CPL - ComPLement register A
        case 0x2F:
            a_ = ~a_;
            f_ |= FLAG_N | FLAG_H;
            break;
        // JR NC,i8
        case 0x30:
            jr_conditional(!(f_ & FLAG_C));
            break;
        // LD SP,u16
        case 0x31:
            sp_ = read_u16();
            break;
        // LD (HL-),A
        case 0x32:
            bus_.write8(get_hl(), a_);
            set_hl(get_hl() - 1);
            break;
        // INC SP
        case 0x33:
            sp_ += 1;
            break;
        // INC (HL)
        case 0x34: {
            uint8_t value = bus_.read8(get_hl());
            inc(value);
            bus_.write8(get_hl(), value);
            break;
        }
        // DEC (HL)
        case 0x35: {
            uint8_t value = bus_.read8(get_hl());
            dec(value);
            bus_.write8(get_hl(), value);
            break;
        }
        // LD (HL),u8
        case 0x36: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            bus_.write8(get_hl(), value);
            break;
        }
        // SCF - Set Carry Flag
        case 0x37:
            f_ &= FLAG_Z;
            f_ |= FLAG_C;
            break;
        // JR C,i8
        case 0x38:
            jr_conditional(f_ & FLAG_C);
            break;
        // ADD HL,SP
        case 0x39:
            add_hl(sp_);
            break;
        // LD A,(HL-)
        case 0x3A:
            a_ = bus_.read8(get_hl());
            set_hl(get_hl() - 1);
            break;
        // DEC SP
        case 0x3B:
            sp_ -= 1;
            break;
        // INC A
        case 0x3C:
            inc(a_);
            break;
        // DEC A
        case 0x3D:
            dec(a_);
            break;
        // LD A,u8
        case 0x3E:
            a_ = bus_.read8(pc_);
            pc_++;
            break;
        // CCF - Complement Carry Flag
        case 0x3F:
            f_ &= (FLAG_Z | FLAG_C);
            f_ ^= FLAG_C;
            break;


        // -------------- Load Operations --------------

        // LD B,B
        case 0x40:
            b_ = b_;
            break;
        // LD B,C
        case 0x41:
            b_ = c_;
            break;
        // LD B,D
        case 0x42:
            b_ = d_;
            break;
        // LD B,E
        case 0x43:
            b_ = e_;
            break;
        // LD B,H
        case 0x44:
            b_ = h_;
            break;
        // LD B,L
        case 0x45:
            b_ = l_;
            break;
        // LD, B,(HL)
        case 0x46:
            b_ = bus_.read8(get_hl());
            break;
        // LD B,A
        case 0x47:
            b_ = a_;
            break;
        // LD C,B
        case 0x48:
            c_ = b_;
            break;
        // LD C,C
        case 0x49:
            c_ = c_;
            break;
        // LD C,D
        case 0x4A:
            c_ = d_;
            break;
        // LD C,E
        case 0x4B:
            c_ = e_;
            break;
        // LD C,H
        case 0x4C:
            c_ = h_;
            break;
        // LD C,L
        case 0x4D:
            c_ = l_;
            break;
        // LD C,(HL)
        case 0x4E:
            c_ = bus_.read8(get_hl());
            break;
        // LD C,A
        case 0x4F:
            c_ = a_;
            break;
        // LD D,B
        case 0x50:
            d_ = b_;
            break;
        // LD D,C
        case 0x51:
            d_ = c_;
            break;
        // LD D,D
        case 0x52:
            d_ = d_;
            break;
        // LD D,E
        case 0x53:
            d_ = e_;
            break;
        // LD D,H
        case 0x54:
            d_ = h_;
            break;
        // LD D,L
        case 0x55:
            d_ = l_;
            break;
        // LD D,(HL)
        case 0x56:
            d_ = bus_.read8(get_hl());
            break;
        // LD D,A
        case 0x57:
            d_ = a_;
            break;
        // LD E,B
        case 0x58:
            e_ = b_;
            break;
        // LD E,C
        case 0x59:
            e_ = c_;
            break;
        // LD E,D
        case 0x5A:
            e_ = d_;
            break;
        // LD E,E
        case 0x5B:
            e_ = e_;
            break;
        // LD E,H
        case 0x5C:
            e_ = h_;
            break;
        // LD E,L
        case 0x5D:
            e_ = l_;
            break;
        // LD E,(HL)
        case 0x5E:
            e_ = bus_.read8(get_hl());
            break;
        // LD E,A
        case 0x5F:
            e_ = a_;
            break;
        // LD H,B
        case 0x60:
            h_ = b_;
            break;
        // LD H,C
        case 0x61:
            h_ = c_;
            break;
        // LD H,D
        case 0x62:
            h_ = d_;
            break;
        // LD H,E
        case 0x63:
            h_ = e_;
            break;
        // LD H,H
        case 0x64:
            h_ = h_;
            break;
        // LD H,L
        case 0x65:
            h_ = l_;
            break;
        // LD H,(HL)
        case 0x66:
            h_ = bus_.read8(get_hl());
            break;
        // LD H,A
        case 0x67:
            h_ = a_;
            break;
        // LD L,B
        case 0x68:
            l_ = b_;
            break;
        // LD L,C
        case 0x69:
            l_ = c_;
            break;
        // LD L,D
        case 0x6A:
            l_ = d_;
            break;
        // LD L,E
        case 0x6B:
            l_ = e_;
            break;
        // LD L,H
        case 0x6C:
            l_ = h_;
            break;
        // LD L,L
        case 0x6D:
            l_ = l_;
            break;
        // LD L,(HL)
        case 0x6E:
            l_ = bus_.read8(get_hl());
            break;
        // LD L,A
        case 0x6F:
            l_ = a_;
            break;
        // LD (HL),B
        case 0x70:
            bus_.write8(get_hl(), b_);
            break;
        // LD (HL),C
        case 0x71:
            bus_.write8(get_hl(), c_);
            break;
        // LD (HL),D
        case 0x72:
            bus_.write8(get_hl(), d_);
            break;
        // LD (HL),E
        case 0x73:
            bus_.write8(get_hl(), e_);
            break;
        // LD (HL),H
        case 0x74:
            bus_.write8(get_hl(), h_);
            break;
        // LD (HL),L
        case 0x75:
            bus_.write8(get_hl(), l_);
            break;
        // HALT: Need to come back to this once interrupts are implemented
        case 0x76:
            break;
        // LD (HL),A
        case 0x77:
            bus_.write8(get_hl(), a_);
            break;
        // LD A,B
        case 0x78:
            a_ = b_;
            break;
        // LD A,C
        case 0x79:
            a_ = c_;
            break;
        // LD A,D
        case 0x7A:
            a_ = d_;
            break;
        // LD A,E
        case 0x7B:
            a_ = e_;
            break;
        // LD A,H
        case 0x7C:
            a_ = h_;
            break;
        // LD A,L
        case 0x7D:
            a_ = l_;
            break;
        // LD A,(HL)
        case 0x7E:
            a_ = bus_.read8(get_hl());
            break;
        // LD A,A
        case 0x7F:
            a_ = a_;
            break;

        // -------------- Arithmetic Operations --------------

        // ADD A,B
        case 0x80:
            add(b_);
            break;
        // ADD A,C
        case 0x81:
            add(c_);
            break;
        // ADD A,D
        case 0x82:
            add(d_);
            break;
        // ADD A,E
        case 0x83:
            add(e_);
            break;
        // ADD A,H
        case 0x84:
            add(h_);
            break;
        // ADD A,L
        case 0x85:
            add(l_);
            break;
        // ADD A,(HL)
        case 0x86:
            add(bus_.read8(get_hl()));
            break;
        // ADD A,A
        case 0x87:
            add(a_);
            break;
        // ADC A,B
        case 0x88:
            adc(b_);
            break;
        // ADC A,C
        case 0x89:
            adc(c_);
            break;
        // ADC A,D
        case 0x8A:
            adc(d_);
            break;
        // ADC A,E
        case 0x8B:
            adc(e_);
            break;
        // ADC A,H
        case 0x8C:
            adc(h_);
            break;
        // ADC A,L
        case 0x8D:
            adc(l_);
            break;
        // ADC A,(HL)
        case 0x8E:
            adc(bus_.read8(get_hl()));
            break;
        // ADC A,A
        case 0x8F:
            adc(a_);
            break;
        // SUB A,B
        case 0x90:
            sub(b_);
            break;
        // SUB A,C
        case 0x91:
            sub(c_);
            break;
        // SUB A,D
        case 0x92:
            sub(d_);
            break;
        // SUB A,E
        case 0x93:
            sub(e_);
            break;
        // SUB A,H
        case 0x94:
            sub(h_);
            break;
        // SUB A,L
        case 0x95:
            sub(l_);
            break;
        // SUB A,(HL)
        case 0x96:
            sub(bus_.read8(get_hl()));
            break;
        // SUB A,A
        case 0x97:
            sub(a_);
            break;
        // SBC A,B
        case 0x98:
            sbc(b_);
            break;
        // SBC A,C
        case 0x99:
            sbc(c_);
            break;
        // SBC A,D
        case 0x9A:
            sbc(d_);
            break;
        // SBC A,E
        case 0x9B:
            sbc(e_);
            break;
        // SBC A,H
        case 0x9C:
            sbc(h_);
            break;
        // SBC A,L
        case 0x9D:
            sbc(l_);
            break;
        // SBC A,(HL)
        case 0x9E:
            sbc(bus_.read8(get_hl()));
            break;
        // SBC A,A
        case 0x9F:
            sbc(a_);
            break;

        // -------------- Logical Operations --------------

        // AND A,B
        case 0xA0:
            and_(b_);
            break;
        // AND A,C
        case 0xA1:
            and_(c_);
            break;
        // AND A,D
        case 0xA2:
            and_(d_);
            break;
        // AND A,E
        case 0xA3:
            and_(e_);
            break;
        // AND A,H
        case 0xA4:
            and_(h_);
            break;
        // AND A,L
        case 0xA5:
            and_(l_);
            break;
        // AND A,(HL)
        case 0xA6:
            and_(bus_.read8(get_hl()));
            break;
        // AND A,A
        case 0xA7:
            and_(a_);
            break;
        // XOR A,B
        case 0xA8:
            xor_(b_);
            break;
        // XOR A,C
        case 0xA9:
            xor_(c_);
            break;
        // XOR A,D
        case 0xAA:
            xor_(d_);
            break;
        // XOR A,E
        case 0xAB:
            xor_(e_);
            break;
        // XOR A,H
        case 0xAC:
            xor_(h_);
            break;
        // XOR A,L
        case 0xAD:
            xor_(l_);
            break;
        // XOR A,(Hl)
        case 0xAE:
            xor_(bus_.read8(get_hl()));
            break;
        // XOR A,A
        case 0xAF:
            xor_(a_);
            break;
        // OR A,B
        case 0xB0:
            or_(b_);
            break;
        // OR A,C
        case 0xB1:
            or_(c_);
            break;
        // OR A,D
        case 0xB2:
            or_(d_);
            break;
        // OR A,E
        case 0xB3:
            or_(e_);
            break;
        // OR A,H
        case 0xB4:
            or_(h_);
            break;
        // OR A,L
        case 0xB5:
            or_(l_);
            break;
        // OR A,(HL)
        case 0xB6:
            or_(bus_.read8(get_hl()));
            break;
        // OR A,A
        case 0xB7:
            or_(a_);
            break;
        // CP A,B
        case 0xB8:
            cp(b_);
            break;
        // CP A,C
        case 0xB9:
            cp(c_);
            break;
        // CP A,D
        case 0xBA:
            cp(d_);
            break;
        // CP A,E
        case 0xBB:
            cp(e_);
            break;
        // CP A,H
        case 0xBC:
            cp(h_);
            break;
        // CP A,L
        case 0xBD:
            cp(l_);
            break;
        // CP A,(HL)
        case 0xBE:
            cp(bus_.read8(get_hl()));
            break;
        // CP A,A
        case 0xBF:
            cp(a_);
            break;
        // RET NZ
        case 0xC0:
            ret_conditional(!(f_ & FLAG_Z));
            break;
        // POP BC
        case 0xC1:
            set_bc(pop16());
            break;
        // JP NZ,u16
        case 0xC2:
            jp_conditional(!(f_ & FLAG_Z));
            break;
        // JP u16
        case 0xC3:
            jp_conditional(true);
            break;
        // CALL NZ,u16
        case 0xC4:
            call_conditional(!(f_ & FLAG_Z));
            break;
        // PUSH BC
        case 0xC5:
            push16(get_bc());
            break;
        // ADD A,u8
        case 0xC6: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            add(value);
            break;
        }
        // RST 00h
        case 0xC7:
            rst(0x00);
            break;
            // RET Z
        case 0xC8:
            ret_conditional(f_ & FLAG_Z);
            break;
        // RET
        case 0xC9:
            pc_ = pop16();
            break;
        // JP Z,u16
        case 0xCA:
            jp_conditional(f_ & FLAG_Z);
            break;
        // CB prefix - Execute a CB prefixed opcode
        case 0xCB: {
            uint8_t cb_opcode = bus_.read8(pc_);
            pc_++;
            execute_cb(cb_opcode);
            break;
        }
        // CALL Z,u16
        case 0xCC:
            call_conditional(f_ & FLAG_Z);
            break;
        // CALL u16
        case 0xCD:
            call_conditional(true);
            break;
        // ADC A,u8
        case 0xCE: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            adc(value);
            break;
        }
        case 0xCF:
            rst(0x08);
            break;
        // RET NC
        case 0xD0:
            ret_conditional(!(f_ & FLAG_C));
            break;
        // POP DE
        case 0xD1:
            set_de(pop16());
            break;
        // JP NC,u16
        case 0xD2:
            jp_conditional(!(f_ & FLAG_C));
            break;
        // CALL NC,u16
        case 0xD4:
            call_conditional(!(f_ & FLAG_C));
            break;
        // PUSH DE
        case 0xD5:
            push16(get_de());
            break;
        // SUB A,u8
        case 0xD6: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            sub(value);
            break;
        }
        // RST 10h
        case 0xD7:
            rst(0x10);
            break;
        // RET C
        case 0xD8:
            ret_conditional(f_ & FLAG_C);
            break;
        // RETI - return and re-enable interrupts
        case 0xD9:
            pc_ = pop16();
            ime_ = true;
            break;
        // JP C,u16
        case 0xDA:
            jp_conditional(f_ & FLAG_C);
            break;
        // CALL C,u16
        case 0xDC:
            call_conditional(f_ & FLAG_C);
            break;
        // SBC A,u8
        case 0xDE: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            sbc(value);
            break;
        }
        // RST 18h
        case 0xDF:
            rst(0x18);
            break;
        // LDH (u8),A
        case 0xE0: {
            uint8_t offset = bus_.read8(pc_);
            pc_++;
            bus_.write8(0xFF00 + offset, a_);
            break;
        }
        // POP HL
        case 0xE1:
            set_hl(pop16());
            break;
        // LD (FF00+C),A
        case 0xE2:
            bus_.write8(0xFF00 + c_, a_);
            break;
        // PUSH HL
        case 0xE5:
            push16(get_hl());
            break;
        // AND A,u8
        case 0xE6: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            and_(value);
            break;
        }
        // RST 20h
        case 0xE7:
            rst(0x20);
            break;
        // ADD SP,i8
        case 0xE8:
            sp_ = add_sp_i8();
            break;
        // JP HL
        case 0xE9:
            pc_ = get_hl();
            break;
            // LD (u16),A
        case 0xEA:
            bus_.write8(read_u16(), a_);
            break;
        // XOR A,u8
        case 0xEE: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            xor_(value);
            break;
        }
        // RST 28h
        case 0xEF:
            rst(0x28);
            break;
        // LDH A,(u8)
        case 0xF0: {
            uint8_t offset = bus_.read8(pc_);
            pc_++;
            a_ = bus_.read8(0xFF00 + offset);
            break;
        }
        // POP AF
        case 0xF1:
            set_af(pop16());
            break;
        // LD A,(FF00+C)
        case 0xF2:
            a_ = bus_.read8(0xFF00 + c_);
            break;
        // DI - disable interrupts
        case 0xF3:
            ime_ = false;
            break;
        // PUSH AF
        case 0xF5:
            push16(get_af());
            break;
        // OR A,u8
        case 0xF6: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            or_(value);
            break;
        }
        // RST 30h
        case 0xF7:
            rst(0x30);
            break;
        // LD HL,SP+i8
        case 0xF8:
            set_hl(add_sp_i8());
            break;
        // LD SP,HL
        case 0xF9:
            sp_ = get_hl();
            break;
        // LD A,(u16)
        case 0xFA:
            a_ = bus_.read8(read_u16());
            break;
            // EI - enable interrupts
        case 0xFB:
            // Dont think this should set immediately, should be delayed by one instruction when
            // sorting out actual instruction timings.
            ime_ = true;
            break;
        // CP A,u8
        case 0xFE: {
            uint8_t value = bus_.read8(pc_);
            pc_++;
            cp(value);
            break;
        }
        // RST 38h
        case 0xFF:
            rst(0x38);
            break;

        default:
            // Unimplemented opcode, come back to this and sort it out.
            break;
    }
}
