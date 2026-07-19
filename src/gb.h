#pragma once

#include "bus.h"
#include "cpu.h"
#include "cartridges/cartridge.h"

#include <string>


class Gameboy {
    public:
        Gameboy();

        bool load_rom(const std::string& filename);
        void step();
        void run(int max_steps);


        // Accessors for debugging for now when doing small hand written tests
        CPU& cpu() { return cpu_; }
        Bus& bus() { return bus_; }

    private:
        Cartridge cartridge_;
        Bus bus_;
        CPU cpu_;
};
