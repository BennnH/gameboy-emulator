#include "gb.h"

#include <iostream>

int main() {
    Gameboy gb;

    // Hand-written program in work RAM (ROM is read-only, so we can't use 0x0100)
    uint16_t addr = 0xC000;
    gb.bus().write8(addr + 0, 0x3E);  // LD A,0x42
    gb.bus().write8(addr + 1, 0x42);
    gb.bus().write8(addr + 2, 0x06);  // LD B,0x10
    gb.bus().write8(addr + 3, 0x10);
    gb.bus().write8(addr + 4, 0x80);  // ADD A,B
    gb.bus().write8(addr + 5, 0x05);  // DEC B
    gb.bus().write8(addr + 6, 0x00);  // NOP

    gb.cpu().set_pc(addr);

    std::cout << "Initial: ";
    gb.cpu().print_state();

    for (int i = 1; i <= 5; i++) {
        gb.step();
        std::cout << "Step " << i << ":  ";
        gb.cpu().print_state();
    }

    return 0;
}
