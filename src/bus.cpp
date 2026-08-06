#include "bus.h"
#include "cartridges/cartridge.h"

#include <cstdio>


Bus::Bus(Cartridge& cartridge) : cartridge_(cartridge) {
}


void Bus::reset() {
    write_io(0xFF40, 0x91); // LCDC: LCD on, BG on, tile data 0x8000, tilemap 0x9800
    write_io(0xFF47, 0xFC); // BGP: standard post-boot background palette
}


uint8_t Bus::read_io(uint16_t address) const {
    // Interupt flag register. Top 3 bits aren't used and always have value '1'
    if (address == 0xFF0F) {
        return io_[address - 0xFF00] | 0xE0;
    }
    if (address == 0xFF00) {
        uint8_t selection = io_[0];
        bool select_dir = !(selection & 0x10);
        bool select_act = !(selection & 0x20);

        uint8_t dir_nibble = (~button_state_) & 0x0F;
        uint8_t act_nibble = (~(button_state_ >> 4)) & 0x0F;

        uint8_t bits = 0x0F;
        if (select_dir) bits &= dir_nibble;
        if (select_act) bits &= act_nibble;

        return 0xC0 | (selection & 0x30) | bits;
    }
    return io_[address - 0xFF00];
}


void Bus::write_io(uint16_t address, uint8_t value) {
    // Serial data transfer, outputs the byte when a transfer is requested so we can debug through terminal output.
    if (address == 0xFF02 && value == 0x81) {
        std::printf("%c", io_[0xFF01 - 0xFF00]);
        // Force output the contents, wouldn't happen without this in an inf loop
        std::fflush(stdout);
        // Clear to signal the transfer is done which is done by actual hardware.
        io_[0xFF02 - 0xFF00] = 0x01;
        return;
    }
    io_[address - 0xFF00] = value;
}


uint8_t Bus::read8(uint16_t address) const {
    // Memory region for rom/cartridge (0x0000 - 0x7FFF)
    if (address <= 0x7FFF) {
        return cartridge_.read(address);
    }

    if ((address >= 0x8000) && (address <= 0x9FFF)) {
        return vram_[address - 0x8000];
    }

    if ((address >= 0xA000) && (address <= 0xBFFF)) {
        // This is for MBC later so just returning a placeholder value for now.
        return 0xFF;
    }

    if ((address >= 0xC000) && (address <= 0xDFFF)) {
        return wram_[address - 0xC000];
    }

    if ((address >= 0xE000) && (address <= 0xFDFF)) {
        // This is a mirror of WRAM
        return wram_[address - 0xE000];
    }

    if ((address >= 0xFE00) && (address <= 0xFE9F)) {
        return oam_[address - 0xFE00];
    }

    if ((address >= 0xFEA0) && (address <= 0xFEFF)) {
        // Nintendo says use of this area is prohibited
        return 0xFF;
    }

    if ((address >= 0xFF00) && (address <= 0xFF7F)) {
        return read_io(address);
    }

    if ((address >= 0xFF80) && (address <= 0xFFFE)) {
        return hram_[address - 0xFF80];
    }

    return ie_;
}


void Bus::write8(uint16_t address, uint8_t value) {
    // For now roms are read only, and we don't write to cartridges so ignore.
    if (address <= 0x7FFF) {
        return;
    }

    if ((address >= 0x8000) && (address <= 0x9FFF)) {
        vram_[address - 0x8000] = value;
    }

    if ((address >= 0xA000) && (address <= 0xBFFF)) {
        // Used for MBC, ignore until later.
    }

    if ((address >= 0xC000) && (address <= 0xDFFF)) {
        wram_[address - 0xC000] = value;
    }

    if ((address >= 0xE000) && (address <= 0xFDFF)) {
        // This is a mirror of WRAM
        wram_[address - 0xE000] = value;
    }

    if ((address >= 0xFE00) && (address <= 0xFE9F)) {
        oam_[address - 0xFE00] = value;
    }

    if ((address >= 0xFEA0) && (address <= 0xFEFF)) {
        // Nintendo says use of this area is prohibited
    }

    if ((address >= 0xFF00) && (address <= 0xFF7F)) {
        write_io(address, value);
    }

    if ((address >= 0xFF80) && (address <= 0xFFFE)) {
        hram_[address - 0xFF80] = value;
    }

    if (address == 0xFFFF) {
        ie_ = value;
    }
}


void Bus::tick(int cycles) {
    cycles_ += cycles;

    // DIV, which is always running
    div_counter_ += cycles;
    // Increment the DIV register if it overflows
    while (div_counter_ >= 256) {
        div_counter_ -= 256;
        io_[0xFF04 - 0xFF00]++;
    }

    // TIMA, only if the enbled bit is on in the TAC register
    if (io_[0xFF07 - 0xFF00] & 0x04){
        tima_counter_ += cycles;
        int tima_threshold = get_tac_speed();

        while (tima_counter_ >= tima_threshold) {
            tima_counter_ -= tima_threshold;
            if (io_[0xFF05 - 0xFF00] == 0xFF) {
                // This increment would make it overflow, so we reset to TMA (the reset value) and raise the timer interupt.
                io_[0xFF05 - 0xFF00] = io_[0xFF06 - 0xFF00]; // TMA
                io_[0xFF0F - 0xFF00] |= 0x04; // raise the timer bit of interupt flag register.
            } else {
                io_[0xFF05 - 0xFF00]++;
            }
        }
    }
}


int Bus::get_tac_speed() const{
    switch (io_[0xFF07 - 0xFF00] & 0x03) {
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


void Bus::set_button_state(uint8_t button_mask, bool pressed) {
    if (pressed) {
        button_state_ |= button_mask;
    } else {
        button_state_ &= ~button_mask;
    }
}
