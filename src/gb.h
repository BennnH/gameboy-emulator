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
        int step();
        void run(int max_steps);
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
