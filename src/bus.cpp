#include "bus.h"
#include "cartridges/cartridge.h"

#include <cstdio>


Bus::Bus(Cartridge& cartridge) : cartridge_(cartridge) {
}

uint8_t Bus::read8(uint16_t address) const {
    // Memory region for rom/cartridge (0x0000 - 0x7FFF)
    if (address <= 0x7FFF) {
        return cartridge_.read(address);
    }

    if (address == 0xFF0F) {
        return this->memory_[address] | 0xE0;
    }
    // For now everything else is just this temp memory array for now
    return this->memory_[address];
}

void Bus::write8(uint16_t address, uint8_t value) {
    // For now roms are read only, and we don't write to cartridges so ignore.
    if (address <= 0x7FFF) {
        return;
    }
    this->memory_[address] = value;

    if (address == 0xFF02 && value == 0x81) {
        std::printf("%c", this->memory_[0xFF01]);
        // Force output the contents, wouldn't happen without this in an inf loop
        std::fflush(stdout);
        // Clear to signal the transfer is done which is done by actual hardware.
        this->memory_[0xFF02] = 0x01;
    }
}

void Bus::tick(int cycles) {
    cycles_ += cycles;

    // DIV, which is always running
    div_counter_ += cycles;
    // Increment the DIV register if it overflows
    while (div_counter_ >= 256) {
        div_counter_ -= 256;
        memory_[0xFF04]++;
    }

    // TIMA, only if the enbled bit is on in the TAC register
    if (memory_[0xFF07] & 0x04){
        tima_counter_ += cycles;
        int tima_threshold = get_tac_speed();

        while (tima_counter_ >= tima_threshold) {
            tima_counter_ -= tima_threshold;
            if (memory_[0xFF05] == 0xFF) {
                // This increment would make it overflow, so we reset to TMA (the reset value) and raise the timer interupt.
                memory_[0xFF05] = memory_[0xFF06]; // TMA
                memory_[0xFF0F] |= 0x04; // raise the timer bit of interupt flag register.
            } else {
                memory_[0xFF05]++;
            }
        }
    }
}


int Bus::get_tac_speed() const{
    switch (memory_[0xFF07] & 0x03) {
        case 0x00:
            return 1024;
        case 0x01:
            return 16;
        case 0x02:
            return 64;
        case 0x03:
            return 256;
    }
    // Just default to this for now to keep compiler happy.
    return 1024;
}
