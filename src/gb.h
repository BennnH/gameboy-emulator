#pragma once

#include "bus.h"
#include "cpu.h"
#include "ppu.h"
#include "cartridges/cartridge.h"

#include <string>


class Gameboy {
    public:
        Gameboy();

        bool load_rom(const std::string& filename);
        void write_save() { cartridge_.write_save(); }
        int step();
        void run_frame();


        CPU& cpu() { return cpu_; }
        Bus& bus() { return bus_; }
        PPU& ppu() { return ppu_; }

    private:
        Cartridge cartridge_;
        Bus bus_;
        CPU cpu_;
        PPU ppu_;
};
