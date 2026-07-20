#include "cpu.h"
#include "bus.h"

// RLC - rotate left circular. Bit 7 wraps around to bit 0 AND goes to carry.
uint8_t CPU::rlc(uint8_t value) {
    uint8_t carry = (value >> 7) & 1;
    uint8_t result = (value << 1) | carry;
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// RRC - rotate right circular. Bit 0 wraps around to bit 7 AND goes to carry.
uint8_t CPU::rrc(uint8_t value) {
    uint8_t carry = value & 1;
    uint8_t result = (value >> 1) | (carry << 7);
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// RL - rotate left through carry. Old carry rotates INTO bit 0, bit 7 becomes new carry.
uint8_t CPU::rl(uint8_t value) {
    uint8_t old_carry = (f_ & FLAG_C) ? 1 : 0;
    uint8_t new_carry = (value >> 7) & 1;
    uint8_t result = (value << 1) | old_carry;
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (new_carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// RR - rotate right through carry. Old carry rotates INTO bit 7, bit 0 becomes new carry.
uint8_t CPU::rr(uint8_t value) {
    uint8_t old_carry = (f_ & FLAG_C) ? 1 : 0;
    uint8_t new_carry = value & 1;
    uint8_t result = (value >> 1) | (old_carry << 7);
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (new_carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// SLA - shift left arithmetic. Bit 0 filled with 0, bit 7 goes to carry.
uint8_t CPU::sla(uint8_t value) {
    uint8_t carry = (value >> 7) & 1;
    uint8_t result = value << 1;
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// SRA - shift right arithmetic. Bit 7 is PRESERVED (sign extend), bit 0 goes to carry.
uint8_t CPU::sra(uint8_t value) {
    uint8_t carry = value & 1;
    uint8_t result = (value >> 1) | (value & 0x80);
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// SWAP - exchange the upper and lower nibbles. Never touches carry.
uint8_t CPU::swap(uint8_t value) {
    uint8_t result = (value << 4) | (value >> 4);
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    return result;
}

// SRL - shift right logical. Bit 7 filled with 0, bit 0 goes to carry.
uint8_t CPU::srl(uint8_t value) {
    uint8_t carry = value & 1;
    uint8_t result = value >> 1;
    f_ = 0;
    if (result == 0) {
        f_ |= FLAG_Z;
    }
    if (carry) {
        f_ |= FLAG_C;
    }
    return result;
}

// BIT - test a bit. Does NOT modify the value, only sets flags.
// Z is set when the tested bit is ZERO. H is always set. Carry is preserved.
void CPU::bit(uint8_t bit_number, uint8_t value) {
    f_ &= FLAG_C;
    f_ |= FLAG_H;
    if (!(value & (1 << bit_number))) {
        f_ |= FLAG_Z;
    }
}

void CPU::execute_cb(uint8_t opcode) {
    switch (opcode) {
        // RLC B
        case 0x00:
            b_ = rlc(b_);
            break;
        // RLC C
        case 0x01:
            c_ = rlc(c_);
            break;
        // RLC D
        case 0x02:
            d_ = rlc(d_);
            break;
        // RLC E
        case 0x03:
            e_ = rlc(e_);
            break;
        // RLC H
        case 0x04:
            h_ = rlc(h_);
            break;
        // RLC L
        case 0x05:
            l_ = rlc(l_);
            break;
        // RLC (HL)
        case 0x06: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), rlc(value));
            break;
        }
        // RLC A
        case 0x07:
            a_ = rlc(a_);
            break;
        // RRC B
        case 0x08:
            b_ = rrc(b_);
            break;
        // RRC C
        case 0x09:
            c_ = rrc(c_);
            break;
        // RRC D
        case 0x0A:
            d_ = rrc(d_);
            break;
        // RRC E
        case 0x0B:
            e_ = rrc(e_);
            break;
        // RRC H
        case 0x0C:
            h_ = rrc(h_);
            break;
        // RRC L
        case 0x0D:
            l_ = rrc(l_);
            break;
        // RRC (HL)
        case 0x0E: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), rrc(value));
            break;
        }
        // RRC A
        case 0x0F:
            a_ = rrc(a_);
            break;
        // RL B
        case 0x10:
            b_ = rl(b_);
            break;
        // RL C
        case 0x11:
            c_ = rl(c_);
            break;
        // RL D
        case 0x12:
            d_ = rl(d_);
            break;
        // RL E
        case 0x13:
            e_ = rl(e_);
            break;
        // RL H
        case 0x14:
            h_ = rl(h_);
            break;
        // RL L
        case 0x15:
            l_ = rl(l_);
            break;
        // RL (HL)
        case 0x16: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), rl(value));
            break;
        }
        // RL A
        case 0x17:
            a_ = rl(a_);
            break;
        // RR B
        case 0x18:
            b_ = rr(b_);
            break;
        // RR C
        case 0x19:
            c_ = rr(c_);
            break;
        // RR D
        case 0x1A:
            d_ = rr(d_);
            break;
        // RR E
        case 0x1B:
            e_ = rr(e_);
            break;
        // RR H
        case 0x1C:
            h_ = rr(h_);
            break;
        // RR L
        case 0x1D:
            l_ = rr(l_);
            break;
        // RR (HL)
        case 0x1E: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), rr(value));
            break;
        }
        // RR A
        case 0x1F:
            a_ = rr(a_);
            break;
        // SLA B
        case 0x20:
            b_ = sla(b_);
            break;
        // SLA C
        case 0x21:
            c_ = sla(c_);
            break;
        // SLA D
        case 0x22:
            d_ = sla(d_);
            break;
        // SLA E
        case 0x23:
            e_ = sla(e_);
            break;
        // SLA H
        case 0x24:
            h_ = sla(h_);
            break;
        // SLA L
        case 0x25:
            l_ = sla(l_);
            break;
        // SLA (HL)
        case 0x26: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), sla(value));
            break;
        }
        // SLA A
        case 0x27:
            a_ = sla(a_);
            break;
        // SRA B
        case 0x28:
            b_ = sra(b_);
            break;
        // SRA C
        case 0x29:
            c_ = sra(c_);
            break;
        // SRA D
        case 0x2A:
            d_ = sra(d_);
            break;
        // SRA E
        case 0x2B:
            e_ = sra(e_);
            break;
        // SRA H
        case 0x2C:
            h_ = sra(h_);
            break;
        // SRA L
        case 0x2D:
            l_ = sra(l_);
            break;
        // SRA (HL)
        case 0x2E: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), sra(value));
            break;
        }
        // SRA A
        case 0x2F:
            a_ = sra(a_);
            break;
        // SWAP B
        case 0x30:
            b_ = swap(b_);
            break;
        // SWAP C
        case 0x31:
            c_ = swap(c_);
            break;
        // SWAP D
        case 0x32:
            d_ = swap(d_);
            break;
        // SWAP E
        case 0x33:
            e_ = swap(e_);
            break;
        // SWAP H
        case 0x34:
            h_ = swap(h_);
            break;
        // SWAP L
        case 0x35:
            l_ = swap(l_);
            break;
        // SWAP (HL)
        case 0x36: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), swap(value));
            break;
        }
        // SWAP A
        case 0x37:
            a_ = swap(a_);
            break;
        // SRL B
        case 0x38:
            b_ = srl(b_);
            break;
        // SRL C
        case 0x39:
            c_ = srl(c_);
            break;
        // SRL D
        case 0x3A:
            d_ = srl(d_);
            break;
        // SRL E
        case 0x3B:
            e_ = srl(e_);
            break;
        // SRL H
        case 0x3C:
            h_ = srl(h_);
            break;
        // SRL L
        case 0x3D:
            l_ = srl(l_);
            break;
        // SRL (HL)
        case 0x3E: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), srl(value));
            break;
        }
        // SRL A
        case 0x3F:
            a_ = srl(a_);
            break;
        // BIT 0,B
        case 0x40:
            bit(0, b_);
            break;
        // BIT 0,C
        case 0x41:
            bit(0, c_);
            break;
        // BIT 0,D
        case 0x42:
            bit(0, d_);
            break;
        // BIT 0,E
        case 0x43:
            bit(0, e_);
            break;
        // BIT 0,H
        case 0x44:
            bit(0, h_);
            break;
        // BIT 0,L
        case 0x45:
            bit(0, l_);
            break;
        // BIT 0,(HL)
        case 0x46:
            bit(0, bus_.read8(get_hl()));
            break;
        // BIT 0,A
        case 0x47:
            bit(0, a_);
            break;
        // BIT 1,B
        case 0x48:
            bit(1, b_);
            break;
        // BIT 1,C
        case 0x49:
            bit(1, c_);
            break;
        // BIT 1,D
        case 0x4A:
            bit(1, d_);
            break;
        // BIT 1,E
        case 0x4B:
            bit(1, e_);
            break;
        // BIT 1,H
        case 0x4C:
            bit(1, h_);
            break;
        // BIT 1,L
        case 0x4D:
            bit(1, l_);
            break;
        // BIT 1,(HL)
        case 0x4E:
            bit(1, bus_.read8(get_hl()));
            break;
        // BIT 1,A
        case 0x4F:
            bit(1, a_);
            break;
        // BIT 2,B
        case 0x50:
            bit(2, b_);
            break;
        // BIT 2,C
        case 0x51:
            bit(2, c_);
            break;
        // BIT 2,D
        case 0x52:
            bit(2, d_);
            break;
        // BIT 2,E
        case 0x53:
            bit(2, e_);
            break;
        // BIT 2,H
        case 0x54:
            bit(2, h_);
            break;
        // BIT 2,L
        case 0x55:
            bit(2, l_);
            break;
        // BIT 2,(HL)
        case 0x56:
            bit(2, bus_.read8(get_hl()));
            break;
        // BIT 2,A
        case 0x57:
            bit(2, a_);
            break;
        // BIT 3,B
        case 0x58:
            bit(3, b_);
            break;
        // BIT 3,C
        case 0x59:
            bit(3, c_);
            break;
        // BIT 3,D
        case 0x5A:
            bit(3, d_);
            break;
        // BIT 3,E
        case 0x5B:
            bit(3, e_);
            break;
        // BIT 3,H
        case 0x5C:
            bit(3, h_);
            break;
        // BIT 3,L
        case 0x5D:
            bit(3, l_);
            break;
        // BIT 3,(HL)
        case 0x5E:
            bit(3, bus_.read8(get_hl()));
            break;
        // BIT 3,A
        case 0x5F:
            bit(3, a_);
            break;
        // BIT 4,B
        case 0x60:
            bit(4, b_);
            break;
        // BIT 4,C
        case 0x61:
            bit(4, c_);
            break;
        // BIT 4,D
        case 0x62:
            bit(4, d_);
            break;
        // BIT 4,E
        case 0x63:
            bit(4, e_);
            break;
        // BIT 4,H
        case 0x64:
            bit(4, h_);
            break;
        // BIT 4,L
        case 0x65:
            bit(4, l_);
            break;
        // BIT 4,(HL)
        case 0x66:
            bit(4, bus_.read8(get_hl()));
            break;
        // BIT 4,A
        case 0x67:
            bit(4, a_);
            break;
        // BIT 5,B
        case 0x68:
            bit(5, b_);
            break;
        // BIT 5,C
        case 0x69:
            bit(5, c_);
            break;
        // BIT 5,D
        case 0x6A:
            bit(5, d_);
            break;
        // BIT 5,E
        case 0x6B:
            bit(5, e_);
            break;
        // BIT 5,H
        case 0x6C:
            bit(5, h_);
            break;
        // BIT 5,L
        case 0x6D:
            bit(5, l_);
            break;
        // BIT 5,(HL)
        case 0x6E:
            bit(5, bus_.read8(get_hl()));
            break;
        // BIT 5,A
        case 0x6F:
            bit(5, a_);
            break;
        // BIT 6,B
        case 0x70:
            bit(6, b_);
            break;
        // BIT 6,C
        case 0x71:
            bit(6, c_);
            break;
        // BIT 6,D
        case 0x72:
            bit(6, d_);
            break;
        // BIT 6,E
        case 0x73:
            bit(6, e_);
            break;
        // BIT 6,H
        case 0x74:
            bit(6, h_);
            break;
        // BIT 6,L
        case 0x75:
            bit(6, l_);
            break;
        // BIT 6,(HL)
        case 0x76:
            bit(6, bus_.read8(get_hl()));
            break;
        // BIT 6,A
        case 0x77:
            bit(6, a_);
            break;
        // BIT 7,B
        case 0x78:
            bit(7, b_);
            break;
        // BIT 7,C
        case 0x79:
            bit(7, c_);
            break;
        // BIT 7,D
        case 0x7A:
            bit(7, d_);
            break;
        // BIT 7,E
        case 0x7B:
            bit(7, e_);
            break;
        // BIT 7,H
        case 0x7C:
            bit(7, h_);
            break;
        // BIT 7,L
        case 0x7D:
            bit(7, l_);
            break;
        // BIT 7,(HL)
        case 0x7E:
            bit(7, bus_.read8(get_hl()));
            break;
        // BIT 7,A
        case 0x7F:
            bit(7, a_);
            break;
        // RES 0,B
        case 0x80:
            b_ &= 0xFE;
            break;
        // RES 0,C
        case 0x81:
            c_ &= 0xFE;
            break;
        // RES 0,D
        case 0x82:
            d_ &= 0xFE;
            break;
        // RES 0,E
        case 0x83:
            e_ &= 0xFE;
            break;
        // RES 0,H
        case 0x84:
            h_ &= 0xFE;
            break;
        // RES 0,L
        case 0x85:
            l_ &= 0xFE;
            break;
        // RES 0,(HL)
        case 0x86: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xFE);
            break;
        }
        // RES 0,A
        case 0x87:
            a_ &= 0xFE;
            break;
        // RES 1,B
        case 0x88:
            b_ &= 0xFD;
            break;
        // RES 1,C
        case 0x89:
            c_ &= 0xFD;
            break;
        // RES 1,D
        case 0x8A:
            d_ &= 0xFD;
            break;
        // RES 1,E
        case 0x8B:
            e_ &= 0xFD;
            break;
        // RES 1,H
        case 0x8C:
            h_ &= 0xFD;
            break;
        // RES 1,L
        case 0x8D:
            l_ &= 0xFD;
            break;
        // RES 1,(HL)
        case 0x8E: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xFD);
            break;
        }
        // RES 1,A
        case 0x8F:
            a_ &= 0xFD;
            break;
        // RES 2,B
        case 0x90:
            b_ &= 0xFB;
            break;
        // RES 2,C
        case 0x91:
            c_ &= 0xFB;
            break;
        // RES 2,D
        case 0x92:
            d_ &= 0xFB;
            break;
        // RES 2,E
        case 0x93:
            e_ &= 0xFB;
            break;
        // RES 2,H
        case 0x94:
            h_ &= 0xFB;
            break;
        // RES 2,L
        case 0x95:
            l_ &= 0xFB;
            break;
        // RES 2,(HL)
        case 0x96: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xFB);
            break;
        }
        // RES 2,A
        case 0x97:
            a_ &= 0xFB;
            break;
        // RES 3,B
        case 0x98:
            b_ &= 0xF7;
            break;
        // RES 3,C
        case 0x99:
            c_ &= 0xF7;
            break;
        // RES 3,D
        case 0x9A:
            d_ &= 0xF7;
            break;
        // RES 3,E
        case 0x9B:
            e_ &= 0xF7;
            break;
        // RES 3,H
        case 0x9C:
            h_ &= 0xF7;
            break;
        // RES 3,L
        case 0x9D:
            l_ &= 0xF7;
            break;
        // RES 3,(HL)
        case 0x9E: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xF7);
            break;
        }
        // RES 3,A
        case 0x9F:
            a_ &= 0xF7;
            break;
        // RES 4,B
        case 0xA0:
            b_ &= 0xEF;
            break;
        // RES 4,C
        case 0xA1:
            c_ &= 0xEF;
            break;
        // RES 4,D
        case 0xA2:
            d_ &= 0xEF;
            break;
        // RES 4,E
        case 0xA3:
            e_ &= 0xEF;
            break;
        // RES 4,H
        case 0xA4:
            h_ &= 0xEF;
            break;
        // RES 4,L
        case 0xA5:
            l_ &= 0xEF;
            break;
        // RES 4,(HL)
        case 0xA6: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xEF);
            break;
        }
        // RES 4,A
        case 0xA7:
            a_ &= 0xEF;
            break;
        // RES 5,B
        case 0xA8:
            b_ &= 0xDF;
            break;
        // RES 5,C
        case 0xA9:
            c_ &= 0xDF;
            break;
        // RES 5,D
        case 0xAA:
            d_ &= 0xDF;
            break;
        // RES 5,E
        case 0xAB:
            e_ &= 0xDF;
            break;
        // RES 5,H
        case 0xAC:
            h_ &= 0xDF;
            break;
        // RES 5,L
        case 0xAD:
            l_ &= 0xDF;
            break;
        // RES 5,(HL)
        case 0xAE: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xDF);
            break;
        }
        // RES 5,A
        case 0xAF:
            a_ &= 0xDF;
            break;
        // RES 6,B
        case 0xB0:
            b_ &= 0xBF;
            break;
        // RES 6,C
        case 0xB1:
            c_ &= 0xBF;
            break;
        // RES 6,D
        case 0xB2:
            d_ &= 0xBF;
            break;
        // RES 6,E
        case 0xB3:
            e_ &= 0xBF;
            break;
        // RES 6,H
        case 0xB4:
            h_ &= 0xBF;
            break;
        // RES 6,L
        case 0xB5:
            l_ &= 0xBF;
            break;
        // RES 6,(HL)
        case 0xB6: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0xBF);
            break;
        }
        // RES 6,A
        case 0xB7:
            a_ &= 0xBF;
            break;
        // RES 7,B
        case 0xB8:
            b_ &= 0x7F;
            break;
        // RES 7,C
        case 0xB9:
            c_ &= 0x7F;
            break;
        // RES 7,D
        case 0xBA:
            d_ &= 0x7F;
            break;
        // RES 7,E
        case 0xBB:
            e_ &= 0x7F;
            break;
        // RES 7,H
        case 0xBC:
            h_ &= 0x7F;
            break;
        // RES 7,L
        case 0xBD:
            l_ &= 0x7F;
            break;
        // RES 7,(HL)
        case 0xBE: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value & 0x7F);
            break;
        }
        // RES 7,A
        case 0xBF:
            a_ &= 0x7F;
            break;
        // SET 0,B
        case 0xC0:
            b_ |= 0x01;
            break;
        // SET 0,C
        case 0xC1:
            c_ |= 0x01;
            break;
        // SET 0,D
        case 0xC2:
            d_ |= 0x01;
            break;
        // SET 0,E
        case 0xC3:
            e_ |= 0x01;
            break;
        // SET 0,H
        case 0xC4:
            h_ |= 0x01;
            break;
        // SET 0,L
        case 0xC5:
            l_ |= 0x01;
            break;
        // SET 0,(HL)
        case 0xC6: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x01);
            break;
        }
        // SET 0,A
        case 0xC7:
            a_ |= 0x01;
            break;
        // SET 1,B
        case 0xC8:
            b_ |= 0x02;
            break;
        // SET 1,C
        case 0xC9:
            c_ |= 0x02;
            break;
        // SET 1,D
        case 0xCA:
            d_ |= 0x02;
            break;
        // SET 1,E
        case 0xCB:
            e_ |= 0x02;
            break;
        // SET 1,H
        case 0xCC:
            h_ |= 0x02;
            break;
        // SET 1,L
        case 0xCD:
            l_ |= 0x02;
            break;
        // SET 1,(HL)
        case 0xCE: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x02);
            break;
        }
        // SET 1,A
        case 0xCF:
            a_ |= 0x02;
            break;
        // SET 2,B
        case 0xD0:
            b_ |= 0x04;
            break;
        // SET 2,C
        case 0xD1:
            c_ |= 0x04;
            break;
        // SET 2,D
        case 0xD2:
            d_ |= 0x04;
            break;
        // SET 2,E
        case 0xD3:
            e_ |= 0x04;
            break;
        // SET 2,H
        case 0xD4:
            h_ |= 0x04;
            break;
        // SET 2,L
        case 0xD5:
            l_ |= 0x04;
            break;
        // SET 2,(HL)
        case 0xD6: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x04);
            break;
        }
        // SET 2,A
        case 0xD7:
            a_ |= 0x04;
            break;
        // SET 3,B
        case 0xD8:
            b_ |= 0x08;
            break;
        // SET 3,C
        case 0xD9:
            c_ |= 0x08;
            break;
        // SET 3,D
        case 0xDA:
            d_ |= 0x08;
            break;
        // SET 3,E
        case 0xDB:
            e_ |= 0x08;
            break;
        // SET 3,H
        case 0xDC:
            h_ |= 0x08;
            break;
        // SET 3,L
        case 0xDD:
            l_ |= 0x08;
            break;
        // SET 3,(HL)
        case 0xDE: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x08);
            break;
        }
        // SET 3,A
        case 0xDF:
            a_ |= 0x08;
            break;
        // SET 4,B
        case 0xE0:
            b_ |= 0x10;
            break;
        // SET 4,C
        case 0xE1:
            c_ |= 0x10;
            break;
        // SET 4,D
        case 0xE2:
            d_ |= 0x10;
            break;
        // SET 4,E
        case 0xE3:
            e_ |= 0x10;
            break;
        // SET 4,H
        case 0xE4:
            h_ |= 0x10;
            break;
        // SET 4,L
        case 0xE5:
            l_ |= 0x10;
            break;
        // SET 4,(HL)
        case 0xE6: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x10);
            break;
        }
        // SET 4,A
        case 0xE7:
            a_ |= 0x10;
            break;
        // SET 5,B
        case 0xE8:
            b_ |= 0x20;
            break;
        // SET 5,C
        case 0xE9:
            c_ |= 0x20;
            break;
        // SET 5,D
        case 0xEA:
            d_ |= 0x20;
            break;
        // SET 5,E
        case 0xEB:
            e_ |= 0x20;
            break;
        // SET 5,H
        case 0xEC:
            h_ |= 0x20;
            break;
        // SET 5,L
        case 0xED:
            l_ |= 0x20;
            break;
        // SET 5,(HL)
        case 0xEE: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x20);
            break;
        }
        // SET 5,A
        case 0xEF:
            a_ |= 0x20;
            break;
        // SET 6,B
        case 0xF0:
            b_ |= 0x40;
            break;
        // SET 6,C
        case 0xF1:
            c_ |= 0x40;
            break;
        // SET 6,D
        case 0xF2:
            d_ |= 0x40;
            break;
        // SET 6,E
        case 0xF3:
            e_ |= 0x40;
            break;
        // SET 6,H
        case 0xF4:
            h_ |= 0x40;
            break;
        // SET 6,L
        case 0xF5:
            l_ |= 0x40;
            break;
        // SET 6,(HL)
        case 0xF6: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x40);
            break;
        }
        // SET 6,A
        case 0xF7:
            a_ |= 0x40;
            break;
        // SET 7,B
        case 0xF8:
            b_ |= 0x80;
            break;
        // SET 7,C
        case 0xF9:
            c_ |= 0x80;
            break;
        // SET 7,D
        case 0xFA:
            d_ |= 0x80;
            break;
        // SET 7,E
        case 0xFB:
            e_ |= 0x80;
            break;
        // SET 7,H
        case 0xFC:
            h_ |= 0x80;
            break;
        // SET 7,L
        case 0xFD:
            l_ |= 0x80;
            break;
        // SET 7,(HL)
        case 0xFE: {
            uint8_t value = bus_.read8(get_hl());
            bus_.write8(get_hl(), value | 0x80);
            break;
        }
        // SET 7,A
        case 0xFF:
            a_ |= 0x80;
            break;
        default:
            // Every CB opcode is valid, so this should be unreachable.
            break;
    }
}
