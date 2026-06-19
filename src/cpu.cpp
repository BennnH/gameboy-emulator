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


        default:
            // Unimplemented opcode, come back to this and sort it out.
            break;
    }
}
