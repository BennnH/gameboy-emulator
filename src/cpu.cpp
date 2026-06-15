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

        default:
            // Unimplemented opcode, come back to this and sort it out.
            break;
    }
}
