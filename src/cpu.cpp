#include "cpu.h"
#include "bus.h"

CPU::CPU(Bus& bus) : bus_(bus) {

}

void CPU::step() {
    uint8_t opcode = bus_.read8(pc_);
    pc_++;

    switch (opcode) {
        case 0x00:
            break;

        // ld opcodes, sorting by families for now as it will get confusing

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
            bus_.write8(get_hl(), h_);
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



        default:
            // Unimplemented opcode, come back to this and sort it out.
            break;
    }
}
